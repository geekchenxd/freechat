#ifndef _USER_LIST_H_
#define _USER_LIST_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MAXNAMESIZE 50
#define MAXSIGNATURESIZE 200

struct user_list {
	bool is_room;
	bool online;
	uint16_t addr;
	char nickname[MAXNAMESIZE];
	char per_signature[MAXSIGNATURESIZE];
	uint8_t ip[16];
	uint16_t port;
	struct user_list *next;
};

struct user_list *user_list_init_user(
		char *nickname, char *ip, uint16_t port,
		uint16_t addr);
void user_list_add(struct user_list *head,
		struct user_list *new);
struct user_list *user_list_find(struct user_list *head, char *nickname);
void user_list_del(struct user_list *head, char *nickname);
void user_list_destroy(struct user_list *head);

#endif
