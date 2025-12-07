#include <stdio.h>
#include "../database.h"
#include "common.h"

void handle_get_teachers(struct mg_connection *nc, int ev, void *ev_data) {
    struct mg_http_message *hm = (struct mg_http_message *)ev_data;
    (void)hm;

    const char *sql = "SELECT 用户ID, 用户名, 真实姓名 FROM 用户表 WHERE 角色 = 2 ORDER BY 用户ID;";
    sqlite3_stmt *stmt;
    cJSON *array = cJSON_CreateArray();

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cJSON *obj = cJSON_CreateObject();
            cJSON_AddNumberToObject(obj, "id", sqlite3_column_int(stmt, 0));
            cJSON_AddStringToObject(obj, "username", (const char*)sqlite3_column_text(stmt, 1));
            cJSON_AddStringToObject(obj, "realName", (const char*)sqlite3_column_text(stmt, 2));
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