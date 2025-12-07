#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../database.h"
#include "common.h"

// ==========================================
// PHẦN 1: QUẢN LÝ HỌC KỲ (TERM)
// ==========================================

// GET /api/terms - Lấy danh sách học kỳ
void handle_get_terms(struct mg_connection *nc, int ev, void *ev_data, void *fn_data) {
    (void)ev; (void)fn_data;
    
    // Sắp xếp ID giảm dần để lấy học kỳ mới nhất lên đầu
    const char *sql = "SELECT 学期ID, 学期名称 FROM 学期表 ORDER BY 学期ID DESC;";
    
    sqlite3_stmt *stmt;
    cJSON *array = cJSON_CreateArray();

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cJSON *obj = cJSON_CreateObject();
            cJSON_AddNumberToObject(obj, "id", sqlite3_column_int(stmt, 0));
            cJSON_AddStringToObject(obj, "name", (const char*)sqlite3_column_text(stmt, 1));
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

// POST /api/terms/add - Thêm học kỳ mới (Optional)
void handle_add_term(struct mg_connection *nc, int ev, void *ev_data, void *fn_data) {
    (void)ev; (void)fn_data;
    struct mg_http_message *hm = (struct mg_http_message *)ev_data;
    
    // Parse JSON thủ công để lấy "name"
    char name[64] = {0};
    if (hm->body.len > 0) {
        char *body_str = malloc(hm->body.len + 1);
        if (body_str) {
            memcpy(body_str, hm->body.buf, hm->body.len);
            body_str[hm->body.len] = 0;
            cJSON *json = cJSON_Parse(body_str);
            if (json) {
                cJSON *n = cJSON_GetObjectItem(json, "name");
                if (cJSON_IsString(n) && n->valuestring) {
                    strncpy(name, n->valuestring, sizeof(name) - 1);
                }
                cJSON_Delete(json);
            }
            free(body_str);
        }
    }

    if (strlen(name) == 0) {
        send_error(nc, 400, "Tên học kỳ không được để trống");
        return;
    }

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, "INSERT INTO 学期表 (学期名称) VALUES (?);", -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cJSON *root = cJSON_CreateObject();
            cJSON_AddNumberToObject(root, "code", 200);
            cJSON_AddStringToObject(root, "msg", "Thêm học kỳ thành công");
            send_json(nc, root);
            cJSON_Delete(root);
        } else {
            send_error(nc, 500, "Học kỳ đã tồn tại hoặc lỗi DB");
        }
        sqlite3_finalize(stmt);
    }
}

// ==========================================
// PHẦN 2: QUẢN LÝ KỲ THI (EXAM)
// ==========================================

// GET /api/exams - Lấy danh sách kỳ thi
void handle_get_exams(struct mg_connection *nc, int ev, void *ev_data, void *fn_data) {
    (void)ev; (void)fn_data;
    
    const char *sql = "SELECT e.考试ID, e.考试名称, e.考试日期, t.学期名称 "
                      "FROM 考试类型表 e "
                      "JOIN 学期表 t ON e.学期ID = t.学期ID "
                      "ORDER BY e.考试日期 DESC;";
    
    sqlite3_stmt *stmt;
    cJSON *array = cJSON_CreateArray();

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cJSON *obj = cJSON_CreateObject();
            cJSON_AddNumberToObject(obj, "id", sqlite3_column_int(stmt, 0));
            cJSON_AddStringToObject(obj, "name", (const char*)sqlite3_column_text(stmt, 1));
            cJSON_AddStringToObject(obj, "date", (const char*)sqlite3_column_text(stmt, 2));
            cJSON_AddStringToObject(obj, "termName", (const char*)sqlite3_column_text(stmt, 3));
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