#ifndef _H_HIS_CMD_H_
#define _H_HIS_CMD_H_

#define MAXCMDNUM 10
#define MAXCMDLENGTH 20

struct cmd {
	int index;
	char cmd[MAXCMDLENGTH];
};

typedef struct double_link {
	struct cmd cmd;
	struct double_link *prev;
	struct double_link *next;
} command;


void cmd_link_init(command *head);
void cmd_link_destroy(command **head);
bool cmd_link_add(command **head, char *cmd);
command *cmd_link_find(command *head, char *cmd);
command *cmd_link_find_by_index(command *head, int idx);
bool cmd_link_del(command *head, char *cmd);
command *cmd_link_get_current();

#endif
