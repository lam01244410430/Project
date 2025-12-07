#include "../database.h"
#include "common.h"
#include "cjson/cJSON.h"

void handle_subject_summary(struct mg_connection *nc, struct http_message *hm) {
    const char *sql =
        "SELECT k.科目名称, "
        "   COALESCE(AVG(s.分数),0) AS 平均分, "
        "   COALESCE(SUM(CASE WHEN s.分数 >= 60 THEN 1 ELSE 0 END)*100.0/NULLIF(COUNT(s.分数),0),0) AS 及格率, "
        "   COALESCE(SUM(CASE WHEN s.分数 >= 90 THEN 1 ELSE 0 END)*100.0/NULLIF(COUNT(s.分数),0),0) AS 优秀率, "
        "   COALESCE(SUM(CASE WHEN s.分数 >= 95 THEN 1 ELSE 0 END)*100.0/NULLIF(COUNT(s.分数),0),0) AS 特优率 "
        "FROM 科目表 k LEFT JOIN 成绩表 s ON k.科目ID = s.科目ID "
        "GROUP BY k.科目ID ORDER BY 平均分 DESC;";

    sqlite3_stmt *stmt;
    cJSON *array = cJSON_CreateArray();

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cJSON *obj = cJSON_CreateObject();
            cJSON_AddStringToObject(obj, "subject", (const char*)sqlite3_column_text(stmt, 0));
            cJSON_AddNumberToObject(obj, "avg", sqlite3_column_double(stmt, 1));
            cJSON_AddNumberToObject(obj, "passRate", sqlite3_column_double(stmt, 2));
            cJSON_AddNumberToObject(obj, "excellentRate", sqlite3_column_double(stmt, 3));
            cJSON_AddNumberToObject(obj, "superRate", sqlite3_column_double(stmt, 4));
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