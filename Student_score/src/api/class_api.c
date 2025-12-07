// src/api/class_api.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../database.h"
#include "common.h"

// GET /api/classes - Lấy danh sách lớp
void handle_classes(struct mg_connection *nc, int ev, void *ev_data, void *fn_data) {
    (void)ev; (void)fn_data;
    const char *sql = "SELECT 班级ID, 班级名称, 年级, "
                      "(SELECT 真实姓名 FROM 用户表 WHERE 用户ID = 班主任ID) AS 班主任 "
                      "FROM 班级表 WHERE 状态 = 1 ORDER BY 班级名称;";

    sqlite3_stmt *stmt;
    cJSON *array = cJSON_CreateArray();

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cJSON *obj = cJSON_CreateObject();
            cJSON_AddNumberToObject(obj, "id", sqlite3_column_int(stmt, 0));
            cJSON_AddStringToObject(obj, "name", (const char*)sqlite3_column_text(stmt, 1));
            cJSON_AddStringToObject(obj, "grade", sqlite3_column_text(stmt, 2) ? (const char*)sqlite3_column_text(stmt, 2) : "");
            cJSON_AddStringToObject(obj, "headTeacher", sqlite3_column_text(stmt, 3) ? (const char*)sqlite3_column_text(stmt, 3) : "未指定");
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

// POST /api/classes - Thêm lớp mới
void handle_add_class(struct mg_connection *nc, int ev, void *ev_data, void *fn_data) {
    (void)ev; (void)fn_data;
    struct mg_http_message *hm = ev_data;
    char name[32] = {0}, grade[16] = {0};

    mg_http_get_var(&hm->body, "name", name, sizeof(name));
    mg_http_get_var(&hm->body, "grade", grade, sizeof(grade));

    if (strlen(name) == 0 || strlen(grade) == 0) {
        send_error(nc, 400, "班级名称和年级不能为空");
        return;
    }

    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO 班级表 (班级名称, 年级, 状态) VALUES (?, ?, 1);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, grade, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cJSON *root = cJSON_CreateObject();
            cJSON_AddNumberToObject(root, "code", 200);
            cJSON_AddStringToObject(root, "msg", "班级添加成功");
            send_json(nc, root);
            cJSON_Delete(root);
        } else {
            send_error(nc, 500, "班级名称已存在");
        }
        sqlite3_finalize(stmt);
    }
}