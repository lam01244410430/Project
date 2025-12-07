// src/api/subject_api.c
// Subject Management API (Admin only) - English code, Chinese response messages

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../database.h"
#include "common.h"

// GET /api/subjects - Get all subjects list
void handle_get_subjects(struct mg_connection *nc, int ev, void *ev_data, void *fn_data) {
    (void)ev; (void)fn_data;

    const char *sql = "SELECT 科目ID, 科目名称, 满分 FROM 科目表 ORDER BY 科目ID;";

    sqlite3_stmt *stmt;
    cJSON *array = cJSON_CreateArray();

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cJSON *obj = cJSON_CreateObject();
            cJSON_AddNumberToObject(obj, "id", sqlite3_column_int(stmt, 0));
            cJSON_AddStringToObject(obj, "name", (const char*)sqlite3_column_text(stmt, 1));
            cJSON_AddNumberToObject(obj, "fullScore", sqlite3_column_double(stmt, 2));
            cJSON_AddItemToArray(array, obj);
        }
        sqlite3_finalize(stmt);
    }

    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "code", 200);
    cJSON_AddStringToObject(root, "msg", "获取科目列表成功");        // Chinese response
    cJSON_AddItemToObject(root, "data", array);
    send_json(nc, root);
    cJSON_Delete(root);
}

// POST /api/subjects/add - Add new subject
void handle_add_subject(struct mg_connection *nc, int ev, void *ev_data, void *fn_data) {
    (void)ev; (void)fn_data;
    struct mg_http_message *hm = (struct mg_http_message *)ev_data;

    char name[64] = {0};
    double full_score = 100.0;

    // Parse JSON body
    if (hm->body.len > 0) {
        char body[512] = {0};
        size_t len = hm->body.len < sizeof(body)-1 ? hm->body.len : sizeof(body)-1;
        memcpy(body, hm->body.buf, len);
        body[len] = '\0';

        cJSON *json = cJSON_Parse(body);
        if (json) {
            cJSON *n = cJSON_GetObjectItem(json, "name");
            cJSON *f = cJSON_GetObjectItem(json, "fullScore");
            if (cJSON_IsString(n) && n->valuestring) strncpy(name, n->valuestring, sizeof(name)-1);
            if (cJSON_IsNumber(f)) full_score = f->valuedouble;
            cJSON_Delete(json);
        }
    }

    if (strlen(name) == 0) {
        send_error(nc, 400, "科目名称不能为空");
        return;
    }

    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO 科目表 (科目名称, 满分) VALUES (?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 2, full_score);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cJSON *root = cJSON_CreateObject();
            cJSON_AddNumberToObject(root, "code", 200);
            cJSON_AddStringToObject(root, "msg", "添加科目成功");
            send_json(nc, root);
            cJSON_Delete(root);
        } else {
            send_error(nc, 400, "科目已存在");
        }
        sqlite3_finalize(stmt);
    } else {
        send_error(nc, 500, "数据库错误");
    }
}

// POST /api/subjects/update - Update subject
void handle_update_subject(struct mg_connection *nc, int ev, void *ev_data, void *fn_data) {
    (void)ev; (void)fn_data;
    struct mg_http_message *hm = (struct mg_http_message *)ev_data;

    int id = 0;
    char name[64] = {0};
    double full_score = 100.0;

    if (hm->body.len > 0) {
        char body[512] = {0};
        size_t len = hm->body.len < sizeof(body)-1 ? hm->body.len : sizeof(body)-1;
        memcpy(body, hm->body.buf, len);
        body[len] = '\0';

        cJSON *json = cJSON_Parse(body);
        if (json) {
            cJSON *i = cJSON_GetObjectItem(json, "id");
            cJSON *n = cJSON_GetObjectItem(json, "name");
            cJSON *f = cJSON_GetObjectItem(json, "fullScore");
            if (cJSON_IsNumber(i)) id = i->valueint;
            if (cJSON_IsString(n) && n->valuestring) strncpy(name, n->valuestring, sizeof(name)-1);
            if (cJSON_IsNumber(f)) full_score = f->valuedouble;
            cJSON_Delete(json);
        }
    }

    if (id <= 0 || strlen(name) == 0) {
        send_error(nc, 400, "参数错误");
        return;
    }

    sqlite3_stmt *stmt;
    const char *sql = "UPDATE 科目表 SET 科目名称 = ?, 满分 = ? WHERE 科目ID = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 2, full_score);
        sqlite3_bind_int(stmt, 3, id);

        if (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(db) > 0) {
            cJSON *root = cJSON_CreateObject();
            cJSON_AddNumberToObject(root, "code", 200);
            cJSON_AddStringToObject(root, "msg", "修改科目成功");
            send_json(nc, root);
            cJSON_Delete(root);
        } else {
            send_error(nc, 400, "科目不存在");
        }
        sqlite3_finalize(stmt);
    }
}

// POST /api/subjects/delete - Delete subject (only if no scores exist)
void handle_delete_subject(struct mg_connection *nc, int ev, void *ev_data, void *fn_data) {
    (void)ev; (void)fn_data;
    struct mg_http_message *hm = (struct mg_http_message *)ev_data;

    int id = 0;
    if (hm->body.len > 0) {
        char body[128] = {0};
        size_t len = hm->body.len < sizeof(body)-1 ? hm->body.len : sizeof(body)-1;
        memcpy(body, hm->body.buf, len);
        body[len] = '\0';

        cJSON *json = cJSON_Parse(body);
        if (json) {
            cJSON *i = cJSON_GetObjectItem(json, "id");
            if (cJSON_IsNumber(i)) id = i->valueint;
            cJSON_Delete(json);
        }
    }

    if (id <= 0) {
        send_error(nc, 400, "科目ID无效");
        return;
    }

    // Check if subject has any scores
    sqlite3_stmt *check;
    int has_score = 0;
    if (sqlite3_prepare_v2(db, "SELECT 1 FROM 成绩表 WHERE 科目ID = ? LIMIT 1;", -1, &check, NULL) == SQLITE_OK) {
        sqlite3_bind_int(check, 1, id);
        has_score = (sqlite3_step(check) == SQLITE_ROW);
        sqlite3_finalize(check);
    }

    if (has_score) {
        send_error(nc, 400, "该科目已有成绩，无法删除");
        return;
    }

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, "DELETE FROM 科目表 WHERE 科目ID = ?;", -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cJSON *root = cJSON_CreateObject();
            cJSON_AddNumberToObject(root, "code", 200);
            cJSON_AddStringToObject(root, "msg", "删除科目成功");
            send_json(nc, root);
            cJSON_Delete(root);
        }
        sqlite3_finalize(stmt);
    }
}