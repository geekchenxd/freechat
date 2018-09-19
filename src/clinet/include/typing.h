#ifndef _TYPING_H_
#define _TYPING_H_

#include "client.h"

void *typing_func(void *arg);
bool user_is_current(struct user_list *user);
void remove_current_select(struct client *p);

#endif
