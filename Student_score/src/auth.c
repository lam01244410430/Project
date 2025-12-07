#include <string.h>
#include "database.h"

int authenticate(const char *username, const char *password) {
    if (strcmp(username, "admin") == 0 && strcmp(password, "admin123") == 0)
        return 1;
    return 0;
}

int get_user_role(const char *username) {
    sqlite3_stmt *stmt;
    int role = 0;
    const char *sql = "SELECT 角色 FROM 用户表 WHERE 用户名 = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW)
            role = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
    }
    return role;
}

int get_user_id(const char *username) {
    sqlite3_stmt *stmt;
    int id = 0;
    const char *sql = "SELECT 用户ID FROM 用户表 WHERE 用户名 = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW)
            id = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
    }
    return id;
}