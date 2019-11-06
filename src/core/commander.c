#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "debug.h"
#include "commander.h"

static struct list_head cmd_head;
static struct list_head cmd_his;
static cmd *last_cmd = NULL;
static int cmd_histories = 0;

static struct cmd *cmd_find_name(char *name)
{
	struct cmd *c = NULL;
	struct list_head *pos;
	int len = 0;

	if (!name)
		return NULL;

	list_for_each(pos, &cmd_head) {
		c = list_entry(pos, struct cmd, list);
		len = strlen(name) > strlen(c->name) ? strlen(name)
			: strlen(c->name);
		if (!strncmp(name, c->name, len))
			return c;
	}

	return NULL;
}

struct cmd *cmd_his_prev(void)
{
	if (last_cmd->list.prev != (&cmd_his))
		last_cmd = list_entry(last_cmd->list.prev,
				struct cmd, list);
	return last_cmd;
}

struct cmd *cmd_his_next(void)
{
	if (last_cmd->list.next != (&cmd_his))
		last_cmd = list_entry(last_cmd->list.next,
				struct cmd, list);
	return last_cmd;
}

static void update_cmd_history(struct cmd *cmd)
{
	last_cmd = cmd;
	list_add_tail(&cmd->list, &cmd_his);
	
	if (cmd_histories >= MAX_HISTORY_CMD) {
		list_del(cmd_his.next);
	} else {
		cmd_hisories++;
	}
}

char *cmd_help(struct cmd *cmd)
{
	if (cmd)
		return cmd->help_info;
	return NULL;
}

int do_cmd(struct cmd *cmd)
{
	if (!cmd)
		return -EINVAL;

	update_cmd_history(c);

	if (c->do_cmd)
		return c->do_cmd(c);

	return 0;
}

int cmd_dispatch(char *cmd)
{
	struct cmd *c;

	c = cmd_find_name(cmd);
	if (!c) {
		debug(INFO, "cmd type %d not found\n", type);
		return -ENXIO;
	}

	update_cmd_history(c);

	if (c->do_cmd)
		return c->do_cmd(c);

	return 0;
}

static bool cmd_is_registered(struct cmd *cmd)
{
	struct cmd *c = NULL;
	struct list_head *pos;

	list_for_each(pos, &cmd_head) {
		c = list_entry(pos, struct cmd, list);
		if (c == cmd)
			return true;
	}

	return false;
}

int cmd_register(struct cmd *cmd)
{
	if (!cmd || !cmd->name) {
		debug(INFO, "%s:Invalid argumengs\n", __FUNCTION__);
		return -EINVAL;
	}

	if (cmd_is_registered(cmd)) {
		debug(INFO, "cmd already exist\n");
		return -EEXIST;
	}

	list_add(&cmd->list, &cmd_head);

	return 0;
}

void cmd_unregister(struct cmd *cmd)
{
	if (cmd) {
		if (cmd->list.prev == (&cmd_his))
			last_cmd = NULL;
		else
			last_cmd = list_entry(last_cmd->list.prev,
					struct cmd, list);

		if (!cmd_is_registered(cmd))
			list_del(&cmd->list);
	}
}

int cmd_init(void)
{
	INIT_LIST_HEAD(&cmd_head);
	INIT_LIST_HEAD(&cmd_his);
	return 0;
}


