#ifndef _H_HIS_CMD_H_
#define _H_HIS_CMD_H_
#include "list.h"

#define MAX_HISTORY_CMD 32
#define MAXCMDLENGTH 32

struct cmd {
	struct list_head list;
	char *name;
	char *help_info;
	void *arg;
	int (*do_cmd)(struct cmd *);
	void *(*help)(struct cmd *);
};

int cmd_dispatch(char *cmd, void *data);
int cmd_register(struct cmd *cmd);
void cmd_unregister(struct cmd *cmd);
char *cmd_help(struct cmd *cmd);
int do_cmd(struct cmd *cmd);
struct cmd *cmd_his_next(void);
struct cmd *cmd_his_prev(void);
int cmd_init(void);

#endif
