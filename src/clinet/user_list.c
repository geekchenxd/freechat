#include "user_list.h"


static struct user_list *user_list_create_user(void)
{
	struct user_list *user = NULL;
	user = (struct user_list *)malloc(sizeof(struct user_list));
	if (!user) {
		fprintf(stderr, "Create user failed\n");
		return NULL;
	}
	memset(user, 0x0, sizeof(struct user_list));

	return user;
}

struct user_list *user_list_init_user(
		char *nickname, char *ip, uint16_t port,
		uint16_t addr)
{
	struct user_list *user = NULL;

	if (!nickname || !ip) {
		fprintf(stderr, "no nickname or invalid ip addr\n");
		return NULL;
	}

	user = user_list_create_user();
	if (user) {
		user->online = true;
		user->addr = addr;
		user->port = port;
		memcpy(&user->nickname[0], nickname, sizeof(user->nickname));
		memcpy(&user->ip[0], ip, sizeof(user->ip));
		user->next = NULL;
	}

	return user;
}

void user_list_add(struct user_list **head, struct user_list *new)
{
	if (!new)
		return;

	if (!*head)
		*head = new;
	else
		(*head)->next = new;
}

struct user_list *user_list_find(struct user_list *head, char *nickname)
{
	if (!head ||!nickname)
		return NULL;

	struct user_list *tmp = NULL;
	
	tmp = head;

	while (tmp) {
		if (!strcmp(nickname, &tmp->nickname[0]))
			break;

		tmp = tmp->next;
	}

	return tmp;
}

struct user_list *find_user_by_id(struct user_list *head, uint16_t addr)
{
	if (!head)
		return NULL;

	if (addr == 65535)
		return NULL;

	struct user_list *tmp = head;

	while (tmp) {
		if (addr == tmp->addr)
			break;
		tmp = tmp->next;
	}

	return tmp;
}

void user_list_del(struct user_list *head, char *nickname)
{
	if (!head || !nickname)
		return;

	struct user_list *user = NULL, *tmp;

	tmp = head;

	if (!strcmp(nickname, &head->nickname[0])) {
		tmp = head;
		head = head->next;
		free(tmp);
		tmp = NULL;
		return;
	}

	while (tmp->next) {
		if (!strcmp(nickname, &tmp->next->nickname[0])) {
			user = tmp->next;
			break;
		}
		tmp = tmp->next;
	}

	if (user) {
		tmp->next = user->next;
		free(user);
		user = NULL;
	}
}

void user_list_destroy(struct user_list *head)
{
	struct user_list *tmp = NULL;

	if (!head) 
		return;

	tmp = head;
	while (head) {
		tmp = head;
		head = head->next;
		free(tmp);
		tmp = NULL;
	}
}

