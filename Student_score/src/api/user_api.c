#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../database.h"
#include "../auth.h"
#include "common.h"

// POST /api/login
void handle_login(struct mg_connection *nc, int ev, void *ev_data, void *fn_data) {
    (void)ev; (void)fn_data;
    struct mg_http_message *hm = (struct mg_http_message *)ev_data;
    char u[64] = {0}, p[64] = {0};
    mg_http_get_var(&hm->body, "username", u, sizeof(u));
    mg_http_get_var(&hm->body, "password", p, sizeof(p));

    // Simple auth check (Trong thực tế nên hash password)
    // Check admin
    if (strcmp(u, "admin") == 0 && strcmp(p, "admin123") == 0) {
        cJSON *root = cJSON_CreateObject();
        cJSON_AddNumberToObject(root, "code", 200);
        cJSON *d = cJSON_CreateObject();
        cJSON_AddStringToObject(d, "token", "admin-token");
        cJSON_AddStringToObject(d, "username", "admin");
        cJSON_AddStringToObject(d, "realName", "Administrator");
        cJSON_AddNumberToObject(d, "role", 1);
        cJSON_AddItemToObject(root, "data", d);
        send_json(nc, root);
        cJSON_Delete(root);
        return;
    }

    // Check DB User
    sqlite3_stmt *stmt;
    const char *sql = "SELECT 用户ID, 角色, 真实姓名 FROM 用户表 WHERE 用户名=? AND 密码哈希=?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, u, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, p, -1, SQLITE_STATIC);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int uid = sqlite3_column_int(stmt, 0);
            int role = sqlite3_column_int(stmt, 1);
            const char *name = (const char*)sqlite3_column_text(stmt, 2);

            cJSON *root = cJSON_CreateObject();
            cJSON_AddNumberToObject(root, "code", 200);
            cJSON *d = cJSON_CreateObject();
            cJSON_AddStringToObject(d, "token", "user-token");
            cJSON_AddNumberToObject(d, "id", uid); // Quan trọng để lấy điểm cá nhân
            cJSON_AddStringToObject(d, "username", u);
            cJSON_AddStringToObject(d, "realName", name ? name : u);
            cJSON_AddNumberToObject(d, "role", role);
            cJSON_AddItemToObject(root, "data", d);
            send_json(nc, root);
            cJSON_Delete(root);
            sqlite3_finalize(stmt);
            return;
        }
        sqlite3_finalize(stmt);
    }

    send_error(nc, 401, "Tên đăng nhập hoặc mật khẩu không đúng");
}

// GET /api/users - Lấy danh sách TẤT CẢ người dùng (Admin, Teacher, Student)
void handle_get_users(struct mg_connection *nc, int ev, void *ev_data, void *fn_data) {
    (void)ev; (void)fn_data;

    // Join bảng Lớp để lấy tên lớp cho học sinh
    const char *sql = 
        "SELECT u.用户ID, u.用户名, u.真实姓名, u.角色, c.班级名称 "
        "FROM 用户表 u "
        "LEFT JOIN 学生扩展表 s ON u.用户ID = s.学生ID "
        "LEFT JOIN 班级表 c ON s.班级ID = c.班级ID "
        "ORDER BY u.用户ID DESC LIMIT 500;"; // Limit để tránh quá tải nếu data quá lớn

    sqlite3_stmt *stmt;
    cJSON *array = cJSON_CreateArray();

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cJSON *obj = cJSON_CreateObject();
            cJSON_AddNumberToObject(obj, "id", sqlite3_column_int(stmt, 0));
            cJSON_AddStringToObject(obj, "username", (const char*)sqlite3_column_text(stmt, 1));
            cJSON_AddStringToObject(obj, "realName", (const char*)sqlite3_column_text(stmt, 2));
            cJSON_AddNumberToObject(obj, "role", sqlite3_column_int(stmt, 3));
            
            const char *className = (const char*)sqlite3_column_text(stmt, 4);
            cJSON_AddStringToObject(obj, "className", className ? className : "");
            
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

// DELETE /api/users/delete - Xóa user (nhận JSON body { "id": 123 })
void handle_delete_user(struct mg_connection *nc, int ev, void *ev_data, void *fn_data) {
    (void)ev; (void)fn_data;
    struct mg_http_message *hm = (struct mg_http_message *)ev_data;
    
    int id = 0;
    // Parse JSON body đơn giản
    if (hm->body.len > 0) {
        char *body_str = malloc(hm->body.len + 1);
        if (body_str) {
            memcpy(body_str, hm->body.buf, hm->body.len);
            body_str[hm->body.len] = 0;
            cJSON *json = cJSON_Parse(body_str);
            if (json) {
                cJSON *id_item = cJSON_GetObjectItem(json, "id");
                if (cJSON_IsNumber(id_item)) id = id_item->valueint;
                cJSON_Delete(json);
            }
            free(body_str);
        }
    }

    if (id <= 0) {
        send_error(nc, 400, "ID không hợp lệ");
        return;
    }

    // Thực hiện xóa (Cascade sẽ tự xóa bên bảng Students/Grades nếu thiết kế DB đúng)
    sqlite3_stmt *stmt;
    const char *sql = "DELETE FROM 用户表 WHERE 用户ID = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cJSON *root = cJSON_CreateObject();
            cJSON_AddNumberToObject(root, "code", 200);
            cJSON_AddStringToObject(root, "msg", "Xóa thành công");
            send_json(nc, root);
            cJSON_Delete(root);
        } else {
            send_error(nc, 500, "Lỗi Database");
        }
        sqlite3_finalize(stmt);
    }
}