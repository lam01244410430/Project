// src/api/score_api.c - FIX CUỐI CÙNG CHO MONGOOSE 7.20+ (DÙNG .buf THAY .ptr)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../database.h"
#include "common.h"

// POST /api/scores/batch - Nhập điểm hàng loạt (Import Excel)
void handle_batch_scores(struct mg_connection *nc, int ev, void *ev_data, void *fn_data) {
    (void)ev; (void)fn_data;
    struct mg_http_message *hm = (struct mg_http_message *)ev_data;

    // Lấy body dưới dạng mg_str (dùng .buf và .len – chuẩn Mongoose 7.x)
    struct mg_str body = hm->body;
    char *json_str = NULL;
    if (body.len > 0) {
        json_str = malloc(body.len + 1);
        if (json_str) {
            memcpy(json_str, body.buf, body.len);  // ← FIX: body.buf thay body.ptr
            json_str[body.len] = '\0';
        }
    }

    if (!json_str || strlen(json_str) == 0) {
        free(json_str);
        send_error(nc, 400, "请求体为空");
        return;
    }

    cJSON *json = cJSON_Parse(json_str);
    free(json_str);
    if (!json || !cJSON_IsArray(json)) {
        if (json) cJSON_Delete(json);
        send_error(nc, 400, "JSON格式错误");
        return;
    }

    int success = 0, failed = 0;
    sqlite3_stmt *stmt;
    const char *sql = "INSERT OR REPLACE INTO 成绩表 (学生ID, 科目ID, 考试ID, 分数, 录入人, 状态) "
                      "VALUES (?, ?, ?, ?, 1, 1);";  // 录入人 = admin (ID=1)

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        send_error(nc, 500, "数据库准备失败");
        cJSON_Delete(json);
        return;
    }

    cJSON *item;
    cJSON_ArrayForEach(item, json) {
        cJSON *sid_json = cJSON_GetObjectItem(item, "studentId");
        cJSON *subid_json = cJSON_GetObjectItem(item, "subjectId");
        cJSON *examid_json = cJSON_GetObjectItem(item, "examId");
        cJSON *score_json = cJSON_GetObjectItem(item, "score");

        if (!cJSON_IsNumber(sid_json) || !cJSON_IsNumber(subid_json) || 
            !cJSON_IsNumber(examid_json) || !cJSON_IsNumber(score_json)) {
            failed++;
            continue;
        }

        int sid = sid_json->valueint;
        int subid = subid_json->valueint;
        int examid = examid_json->valueint;
        double score = score_json->valuedouble;

        sqlite3_bind_int(stmt, 1, sid);
        sqlite3_bind_int(stmt, 2, subid);
        sqlite3_bind_int(stmt, 3, examid);
        sqlite3_bind_double(stmt, 4, score);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            success++;
            // Ghi log (tùy chọn)
            char log_msg[256];
            snprintf(log_msg, sizeof(log_msg), "批量导入成绩: 学生%d, 科目%d, 分数%.1f", sid, subid, score);
            // log_action(1, "ADD_SCORE", log_msg);  // Nếu có hàm log
        } else {
            failed++;
        }
        sqlite3_reset(stmt);
    }
    sqlite3_finalize(stmt);
    cJSON_Delete(json);

    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "code", 200);
    cJSON_AddNumberToObject(root, "success", success);
    cJSON_AddNumberToObject(root, "failed", failed);
    cJSON_AddStringToObject(root, "msg", failed > 0 ? "部分导入失败" : "批量导入完成");
    send_json(nc, root);
    cJSON_Delete(root);
}

// GET /api/scores/student/:id - Xem điểm học sinh cá nhân
void handle_student_scores(struct mg_connection *nc, int ev, void *ev_data, void *fn_data) {
    (void)ev; (void)fn_data;
    struct mg_http_message *hm = (struct mg_http_message *)ev_data;

    // Lấy studentId từ URI: /api/scores/student/123 (dùng .buf và .len)
    struct mg_str uri = hm->uri;
    int student_id = 0;
    if (uri.len > 20) {  // "/api/scores/student/" = 20 ký tự
        // Tìm vị trí số bắt đầu
        const char *id_start = (const char *)uri.buf + 20;  // ← FIX: uri.buf thay uri.ptr
        char id_str[16] = {0};
        int i = 0;
        while (i < 15 && id_start[i] >= '0' && id_start[i] <= '9') {
            id_str[i] = id_start[i];
            i++;
        }
        id_str[i] = '\0';
        student_id = atoi(id_str);
    }

    if (student_id <= 0) {
        send_error(nc, 400, "无效的学生ID");
        return;
    }

    const char *sql = "SELECT k.科目名称, e.考试名称, s.分数, e.考试日期 "
                      "FROM 成绩表 s "
                      "JOIN 科目表 k ON s.科目ID = k.科目ID "
                      "JOIN 考试类型表 e ON s.考试ID = e.考试ID "
                      "WHERE s.学生ID = ? AND s.状态 = 1 "
                      "ORDER BY e.考试日期 DESC;";

    sqlite3_stmt *stmt;
    cJSON *array = cJSON_CreateArray();

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, student_id);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cJSON *obj = cJSON_CreateObject();
            cJSON_AddStringToObject(obj, "subject", (const char*)sqlite3_column_text(stmt, 0));
            cJSON_AddStringToObject(obj, "exam", (const char*)sqlite3_column_text(stmt, 1));
            cJSON_AddNumberToObject(obj, "score", sqlite3_column_double(stmt, 2));
            cJSON_AddStringToObject(obj, "examDate", (const char*)sqlite3_column_text(stmt, 3));
            cJSON_AddItemToArray(array, obj);
        }
        sqlite3_finalize(stmt);
    }

    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "code", 200);
    cJSON_AddItemToObject(root, "data", array);
    send_json(nc, root);
    cJSON_Delete(root);
}