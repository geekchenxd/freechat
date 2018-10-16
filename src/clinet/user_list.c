#include "user_list.h"


static struct user_list *user_list_create_user(void)
{
	struct user_list *user = NULL;
	struct type_user *p = NULL;

	user = (struct user_list *)malloc(sizeof(struct user_list));
	if (!user) {
		fprintf(stderr, "Create user failed\n");
		return NULL;
	}
	memset(user, 0x0, sizeof(struct user_list));
	p = (struct type_user *)malloc(sizeof(struct type_user));
	if (!p) {
		fprintf(stderr, "Create user p failed\n");
		return NULL;
	}
	memset(p, 0x0, sizeof(struct user_list));
	user->user = p;

	return user;
}

struct user_list *user_list_init_user(
		char *nickname, uint16_t addr)
{
	struct user_list *user = NULL;

	if (!nickname) {
		fprintf(stderr, "no nickname or invalid ip addr\n");
		return NULL;
	}

	user = user_list_create_user();
	if (user) {
		user->online = true;
		user->enable = true;
		user->user->id = addr;
		memcpy(&user->user->name[0], nickname, sizeof(user->user->name));
		user->next = NULL;
	}

	return user;
}

void user_list_add(struct user_list **head, struct user_list *new)
{
	struct user_list *tmp = *head;
	if (!new)
		return;

	if (!*head) {
		*head = new;
	} else {
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new;
	}
}

struct user_list *user_list_find(struct user_list *head, char *nickname)
{
	int len = 0;
	if (!head ||!nickname)
		return NULL;

	struct user_list *tmp = NULL;
	
	tmp = head;

	if (strlen(nickname) > MAXNAMESIZE)
		return NULL;

	while (tmp) {
		if (tmp->user->name) {
			len = (strlen(nickname) > strlen(tmp->user->name)) ? 
				strlen(nickname) : strlen(tmp->user->name);

			if (!strncmp(nickname, &tmp->user->name[0], len)) {
				break;
			}
		}

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
		if (addr == tmp->user->id)
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

	if (!strcmp(nickname, &head->user->name[0])) {
		tmp = head;
		head = head->next;
		free(tmp);
		tmp = NULL;
		return;
	}

	while (tmp->next) {
		if (!strcmp(nickname, &tmp->next->user->name[0])) {
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

