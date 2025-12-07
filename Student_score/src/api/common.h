// src/api/common.h
#ifndef COMMON_H
#define COMMON_H

#include "mongoose.h"
#include "cjson/cJSON.h"

void send_json(struct mg_connection *nc, cJSON *json);
void send_error(struct mg_connection *nc, int code, const char *msg);

#ifndef mg_strncmp
static inline int mg_strncmp(const char *s1, const char *s2, size_t n) {
    return strncmp(s1, s2, n);
}
#endif

#endif