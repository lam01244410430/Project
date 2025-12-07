#ifndef AUTH_H
#define AUTH_H

int authenticate(const char *username, const char *password);
int get_user_role(const char *username);
int get_user_id(const char *username);

#endif