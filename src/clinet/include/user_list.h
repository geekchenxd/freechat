#ifndef _USER_LIST_H_
#define _USER_LIST_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "users.h"

#define MAXNAMESIZE 50
#define MAXSIGNATURESIZE 200

struct user_list {
	bool online;
	struct type_user *user;
	struct user_list *next;
};

struct user_list *user_list_init_user(
		char *nickname, uint16_t addr);
void user_list_add(struct user_list **head,
		struct user_list *new);
struct user_list *user_list_find(struct user_list *head, char *nickname);
void user_list_del(struct user_list *head, char *nickname);
void user_list_destroy(struct user_list *head);

#endif
