# Makefile - HOẠT ĐỘNG TRÊN TẤT CẢ HỆ ĐIỀU HÀNH
CC = gcc
CFLAGS = -Wall -Wextra -O2 -I. -Icjson
LIBS = -lsqlite3 -lpthread -ldl

# Windows (MinGW/MSYS2) dùng dòng này thay thế:
# LIBS = -lsqlite3 -lws2_32

all: server

server: main.c mongoose.c cjson/cJSON.c src/*.c src/api/*.c
	$(CC) $(CFLAGS) -o server $^ $(LIBS)

clean:
	rm -f server server.exe database.db *.o

.PHONY: all clean