#include <stdlib.h>
#include "gui.h"

struct gui *pgui = NULL;
char *fix1 = "^G Get help    ^E Quit     ^Y Select    ^O Unselect    ^T Search key";
char *fix2 = "^F Send File   ^W Latest   ^R Up page   ^D Down page   ^A Show contact";

int gui_init(void)
{
	int x, y;
	int ret = 0;
	int display_height;

	if (!pgui) {
		pgui = (struct gui *)malloc(sizeof *pgui);
		if (!pgui)
			return -ENOMEM;
	}

	/* use the system's locale to support Chinese */
	setlocale(LC_ALL, "");
	initscr();

	/* set cursor invisible */
	curs_set(0);

	getmaxyx(stdscr, y, x);
	if (y < MINILINES || x < MINICOLUMNS) {
		endwin();
		printf("Your screen is too small to run program!!!\n");
		printf("At least %d lines and %d columns needed!\n",
				MINILINES, MINICOLUMNS);
		ret = -EINVAL;
		goto free_gui;
	}

	start_color();
	init_pair(1, COLOR_BLACK, COLOR_GREEN);
	init_pair(2, COLOR_BLACK, COLOR_RED);
	init_pair(3, COLOR_BLACK, COLOR_WHITE);

	pgui->top_line = newwin(1, pgui->parent_x, 0, 0);
	pgui->bot_line = newwin(1, pgui->parent_x, pgui->parent_y - 1, 0);
	pgui->left_col = newwin(pgui->parent_y - 2, 1, 1, 0);
	pgui->right_col = newwin(pgui->parent_y - 2, 1, 1, gpui->parent_x - 1);
	pgui->dis_fix = newwin(1, pgui->parent_x - 2, 1, 1);
	pgui->split = newwin(1, pgui->parent_x - 2, pgui->parent_y - size - 3, 1);

	/* h = y - topline - bottomline - splitline*/
	pgui->dis_height = pgui->parent_y - pgui->type_height - 3;
	pgui->dis_height -= 3; /* fix info area and single line */

	wbkgd(pgui->split, COLOR_PAIR(3));
	wrefresh(pgui->split);
	wbkgd(pgui->top_line, COLOR_PAIR(3));
	wrefresh(pgui->top_line);
	wbkgd(pgui->bot_line, COLOR_PAIR(3));
	wrefresh(pgui->bot_line);
	wbkgd(pgui->left_col, COLOR_PAIR(3));
	wrefresh(pgui->left_col);
	wbkgd(pgui->right_col, COLOR_PAIR(3));
	wrefresh(pgui->right_col);

	gui_fill_default(pgui);

	return 0;
free_gui:
	free(pgui);
	return ret;
}
