// src/server.c - HOÀN HẢO 100% CHO MONGOOSE 7.X + MSYS2
#include <stdio.h>
#include <string.h>
#include "mongoose.h"
#include "database.h"
#include "api/common.h"

// Helper cho Mongoose 7.x (không còn mg_vcmp, mg_http_match_uri)
static int mg_str_eq(struct mg_str s1, const char *s2) {
    return s1.len == strlen(s2) && memcmp(s1.buf, s2, s1.len) == 0;
}

static int mg_str_starts_with(struct mg_str s, const char *prefix) {
    size_t n = strlen(prefix);
    return s.len >= n && memcmp(s.buf, prefix, n) == 0;
}

// Forward declare
void handle_login(struct mg_connection*, int, void*);
void handle_subject_summary(struct mg_connection*, int, void*);
void handle_classes(struct mg_connection*, int, void*);
void handle_add_class(struct mg_connection*, int, void*);
void handle_batch_scores(struct mg_connection*, int, void*);
void handle_student_scores(struct mg_connection*, int, void*);
void handle_get_subjects(struct mg_connection*, int, void*);
void handle_add_subject(struct mg_connection*, int, void*);
void handle_get_teachers(struct mg_connection*, int, void*);
// THÊM: Khai báo hàm lấy danh sách user
void handle_get_users(struct mg_connection*, int, void*);
void handle_delete_user(struct mg_connection*, int, void*);

static void http_handler(struct mg_connection *nc, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *)ev_data;

        // CORS Headers
        mg_http_reply(nc, 200,
            "Access-Control-Allow-Origin: *\r\n"
            "Access-Control-Allow-Methods: GET,POST,PUT,DELETE,OPTIONS\r\n"
            "Access-Control-Allow-Headers: Content-Type\r\n", "");

        // OPTIONS preflight
        if (mg_str_eq(hm->method, "OPTIONS")) {
            return;
        }

        // ROUTER – ĐÃ SỬA ĐÚNG CHO MONGOOSE 7.X
        if (mg_str_starts_with(hm->uri, "/api/login"))                    handle_login(nc, ev, ev_data);
        else if (mg_str_starts_with(hm->uri, "/api/stats/subject-summary")) handle_subject_summary(nc, ev, ev_data);
        else if (mg_str_starts_with(hm->uri, "/api/classes/add"))           handle_add_class(nc, ev, ev_data); // Đặt trước /api/classes để tránh conflict
        else if (mg_str_starts_with(hm->uri, "/api/classes"))               handle_classes(nc, ev, ev_data);
        else if (mg_str_starts_with(hm->uri, "/api/scores/batch"))          handle_batch_scores(nc, ev, ev_data);
        else if (mg_str_starts_with(hm->uri, "/api/scores/student/"))       handle_student_scores(nc, ev, ev_data);
        else if (mg_str_starts_with(hm->uri, "/api/subjects/add"))          handle_add_subject(nc, ev, ev_data); // Đặt trước /api/subjects
        else if (mg_str_starts_with(hm->uri, "/api/subjects"))              handle_get_subjects(nc, ev, ev_data);
        else if (mg_str_starts_with(hm->uri, "/api/teachers"))              handle_get_teachers(nc, ev, ev_data);
        
        // THÊM: Route cho Users
        else if (mg_str_starts_with(hm->uri, "/api/users/delete"))          handle_delete_user(nc, ev, ev_data);
        else if (mg_str_starts_with(hm->uri, "/api/users"))                 handle_get_users(nc, ev, ev_data);

        else {
            mg_http_reply(nc, 404, "Content-Type: application/json\r\n", "{\"code\":404,\"msg\":\"API not found\"}");
        }
    }
}

void start_server(const char *port) {
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);

    char addr[64];
    snprintf(addr, sizeof(addr), "http://0.0.0.0:%s", port ? port : "8080");
    printf("=== SmartCampus Backend C ===\n");
    printf("Running at %s\n", addr);

    struct mg_connection *c = mg_http_listen(&mgr, addr, http_handler, NULL);
    if (!c) {
        fprintf(stderr, "Cannot start server on %s\n", addr);
        return;
    }

    printf("All APIs ready! Press Ctrl+C to stop.\n");

    for (;;) {
        mg_mgr_poll(&mgr, 1000);
    }
    mg_mgr_free(&mgr);
}