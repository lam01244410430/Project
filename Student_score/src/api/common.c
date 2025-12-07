#include <stdio.h>
#include <stdlib.h>
#include "common.h"

void send_json(struct mg_connection *nc, cJSON *json) {
    char *str = cJSON_Print(json);
    if (str) {
        mg_http_reply(nc, 200,
            "Content-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\n",
            "%s", str);
        free(str);
    }
}

void send_error(struct mg_connection *nc, int code, const char *msg) {
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "code", code);
    cJSON_AddStringToObject(root, "msg", msg ? msg : "Unknown error");
    send_json(nc, root);
    cJSON_Delete(root);
}