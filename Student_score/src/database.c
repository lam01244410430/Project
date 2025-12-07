#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"

sqlite3 *db = NULL;

void init_database(void) {
    if (sqlite3_open("database.db", &db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        exit(1);
    }

    const char *sql =
        "PRAGMA foreign_keys = ON;"
        "DROP TABLE IF EXISTS 用户表;"
        "CREATE TABLE 用户表 ("
        "    用户ID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    用户名 TEXT NOT NULL UNIQUE,"
        "    密码哈希 TEXT NOT NULL,"
        "    角色 INTEGER NOT NULL,"
        "    真实姓名 TEXT,"
        "    性别 TEXT,"
        "    状态 INTEGER DEFAULT 1,"
        "    创建时间 TEXT DEFAULT (datetime('now','localtime'))"
        ");"
        "DROP TABLE IF EXISTS 班级表;"
        "CREATE TABLE 班级表 ("
        "    班级ID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    班级名称 TEXT NOT NULL UNIQUE,"
        "    年级 TEXT NOT NULL,"
        "    班主任ID INTEGER,"
        "    FOREIGN KEY(班主任ID) REFERENCES 用户表(用户ID)"
        ");"
        "DROP TABLE IF EXISTS 学生扩展表;"
        "CREATE TABLE 学生扩展表 ("
        "    学生ID INTEGER PRIMARY KEY,"
        "    班级ID INTEGER NOT NULL,"
        "    学号 TEXT UNIQUE NOT NULL,"
        "    FOREIGN KEY(学生ID) REFERENCES 用户表(用户ID) ON DELETE CASCADE,"
        "    FOREIGN KEY(班级ID) REFERENCES 班级表(班级ID) ON DELETE CASCADE"
        ");"
        "DROP TABLE IF EXISTS 科目表;"
        "CREATE TABLE 科目表 ("
        "    科目ID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    科目名称 TEXT NOT NULL UNIQUE,"
        "    满分 REAL DEFAULT 100"
        ");"
        "DROP TABLE IF EXISTS 成绩表;"
        "CREATE TABLE 成绩表 ("
        "    成绩ID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    学生ID INTEGER NOT NULL,"
        "    科目ID INTEGER NOT NULL,"
        "    分数 REAL CHECK(分数 >= 0 AND 分数 <= 100),"
        "    FOREIGN KEY(学生ID) REFERENCES 用户表(用户ID) ON DELETE CASCADE,"
        "    FOREIGN KEY(科目ID) REFERENCES 科目表(科目ID) ON DELETE CASCADE,"
        "    UNIQUE(学生ID, 科目ID)" 
        ");"
        "INSERT OR IGNORE INTO 用户表 (用户名, 密码哈希, 角色, 真实姓名) VALUES ('admin', 'admin123', 1, 'Administrator');"
        "INSERT OR IGNORE INTO 科目表 (科目名称, 满分) VALUES ('数学', 100), ('物理', 100), ('英语', 100);"
        "INSERT OR IGNORE INTO 班级表 (班级名称, 年级) VALUES ('高一A班', '10'), ('高一B班', '10');";

    char *err_msg = NULL;
    if (sqlite3_exec(db, sql, NULL, NULL, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        printf("Database initialized successfully!\n");
    }
}