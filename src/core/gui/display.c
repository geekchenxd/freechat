#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "list.h"

#define LEN_LINE 500
#define MAX_LINES 500

struct record {
	struct list_head list;
	int line;
	char string[LEN_LINE];
};

static struct list_head line_head;
static int bottom_line = 0;
static int total_line = 0;

void display_init(void)
{
	INIT_LIST_HEAD(&line_head);
}

void display_raw_fix(struct gui *g, char *ptr)
{
	if (!g || !ptr)
		return;

	wprintw(g->dis_fix, "%s\n", ptr);
}

void display_raw(struct gui *g, char *str)
{
	struct record *rcd = NULL;
	if (!str || !g)
		return;

	rcd = (struct rcd *)malloc(sizeof *rcd);
	if (!rcd)
		return;
	memset(rcd, 0x0, sizeof *rcd);
	memcpy(rcd->string, str, strlen(str));
	list_add_tail(&rcd->list, &line_head);

	total_line++;
	for (i = 0; i < strlen(str); i++)
		if (string[i] == '\n')
			total_line++;

	if (total_line >= MAX_LINES) {
		record = list_entry(line_head.next, struct record, list);
		list_del(&record->list);
		free(record);
	}

	bottom_line = total_line;

	wprintw(g->display, "%s\n", string);
	wrefresh(g->display);
}

void display_single(struct gui *g, char *str)
{
}

void display_up(struct gui *g, int lines)
{
	int desc_line;
	int i;
	struct record *rcd;
	struct list_head *pos;
	int found = 0;

	if (!g || lines <= 0)
		return;

	for (i = 0; i < lines; i++) {
		desc_line = bottom_line - g->dis_height;
		if (desc_line <= 0)
			break;

		list_for_each_prev(pos, &line_head) {
			rcd = list_entry(pos, struct record, list);
			if (rcd->line == desc_line) {
				found = true;
				break;
			}
		}

		if (found) {
			wscrl(g->display, -1);
			mvwprintw(g->display, 0, 0, "%s\n", rcd->string);
			bottom_line--;
		} else {
			break;
		}
	}

	wrefresh(g->display);
}

void display_down(struct gui *g, int lines)
{
	int desc_line;
	int i;
	struct record *rcd;
	struct list_head *pos;
	int found = 0;

	if (!g || lines <= 0)
		return;

	for (i = 0; i < lines; i++) {
		desc_line = bottom_line;
		if (desc_line >= total_line)
			break;

		list_for_each(pos, &line_head) {
			rcd = list_entry(pos, struct record, list);
			if (rcd->line == desc_line) {
				found = true;
				break;
			}
		}

		if (found && bottom_line <= total_line) {
			wscrl(g->display, 0);
			vmove(g->display, g->dis_height - 2, 0);
			wprintw(g->display, "%s\n", rcd->string);
			bottom_line++;
		} else {
			break;
		}
	}

	wrefresh(g->display);
}

void display_search(struct gui *g, char *key)
{
	struct list_head *pos;
	struct record *rcd;

	list_for_each(pos, &line_head) {
		rcd = list_entry(pos, struct record, list);
		if (strstr(rcd->string, key)) {
			found = true;
			break;
		}
	}
}
