#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "cmdhis.h"



static unsigned global_index = 1;
static command *current = NULL;



void cmd_link_init(command *head)
{
	head = NULL;
}

void cmd_link_destroy(command **head)
{
	command *tmp = NULL;

	if (*head == NULL)
		return;
	while (tmp) {
		tmp = (*head)->next;
		free(*head);
		(*head)->prev = NULL;
		*head = tmp;
	}
}

bool cmd_link_add(command **head, char *cmd)
{
	int len = 0;
	command *tmp = NULL;
	command *new = NULL;
	if (!cmd) 
		return false;

	len = strlen(cmd);
	if (len >= MAXCMDLENGTH || len <= 0) {
		return false;
	}

	if (!(*head)) {
		*head = (command *)malloc(sizeof(command));
		if (!(*head))
			return false;
		(*head)->prev = NULL;
		(*head)->next = NULL;
		(*head)->cmd.index = global_index++;
		memcpy((*head)->cmd.cmd, cmd, len);
		(*head)->cmd.cmd[len] = '\0';
		current = *head;
	} else {
		if (global_index > MAXCMDNUM) {
			new = *head;
			*head = (*head)->next;
			(*head)->prev = NULL;
			memset(new->cmd.cmd, 0x0, MAXCMDLENGTH);
			new->cmd.index = global_index;
		} else {
			/*create a new command*/
			new = (command *)malloc(sizeof(command));
			if (!new) {
				return false;
			}
			new->cmd.index = global_index++;
		}
		memcpy(new->cmd.cmd, cmd, len);
		new->cmd.cmd[len] = '\0';

		/*add the new command to the end of the command link*/
		tmp = *head;
		while (tmp->next) {
			if (global_index > MAXCMDNUM)
				tmp->cmd.index--;
			tmp = tmp->next;
		}
		tmp->next = new;
		new->prev = tmp;
		new->next = NULL;
		current = new;
	}

	return true;
}

command *cmd_link_find(command *head, char *cmd)
{
	command *tmp = head;

	if (!head || !cmd)
		return NULL;

	while (tmp) {
		if (!strncmp(cmd, tmp->cmd.cmd, strlen(cmd)))
			return tmp;
		tmp = tmp->next;
	}
	return NULL;
}

command *cmd_link_find_by_index(command *head, int idx)
{
	command *tmp = head;

	if (!head || !idx)
		return NULL;

	while (tmp) {
		if (idx == tmp->cmd.index)
			return tmp;
		tmp = tmp->next;
	}

	return NULL;
}

bool cmd_link_del(command *head, char *cmd)
{
	command *tmp = NULL;
	command *prev = NULL;

	if (!head || !cmd)
		return false;
	tmp = cmd_link_find(head, cmd);
	if (!tmp) {
		return false;
	}
	if (tmp == head) {
		tmp = head->next;
		free(head);

		head = tmp;
		head->prev = NULL;
	} else {
		prev = head;
		while (prev) {
			if (prev->next == tmp)
				break;
			prev = prev->next;
		}
		if (prev) {
			prev->next = tmp->next;
			tmp->next->prev = prev;
			free(tmp);
			tmp->next = NULL;
			tmp->prev = NULL;
		} else {
			return false;
		}
	}

	return true;
}

void cmd_link_show(command *head)
{
	command *tmp = head;
	if (!head)
		return;

	while (tmp) {
		printf("%d : %s\n", tmp->cmd.index, tmp->cmd.cmd);
		tmp = tmp->next;
	}
}

command *cmd_link_get_current()
{
	return current;
}
