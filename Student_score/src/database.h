#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
extern sqlite3 *db;
void init_database(void);
void log_action(int user_id, const char *type, const char *content);

#endif