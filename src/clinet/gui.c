#include "client.h"
#include <locale.h>

int parent_x, parent_y;
int typing_size;

int init_gui(uint8_t size)
{
	if (size <= 0)
		return -2;

	int display_height;
	typing_size = size;

	/*use the system's locale to support Chinese*/
	setlocale(LC_ALL, "");
    initscr();
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_GREEN);
    init_pair(2, COLOR_BLACK, COLOR_RED);
    init_pair(3, COLOR_BLACK, COLOR_WHITE);

    /* get our maximum window dimensions */
    getmaxyx(stdscr, parent_y, parent_x);

    /* draw windows */
    WINDOW *top_line = newwin(1, parent_x, 0, 0);
    WINDOW *bottom_line = newwin(1, parent_x, parent_y - 1, 0);
    WINDOW *left_column = newwin(parent_y - 2, 1, 1, 0);
    WINDOW *right_column = newwin(parent_y - 2, 1, 1, parent_x - 1);

    WINDOW *display = newwin(2, parent_x - 2, 1, 1);
    WINDOW *split_line = newwin(1, parent_x - 2, parent_y - size - 3, 1);

    display_height = parent_y - size - 3; /*top and bottom line and split line*/

    wbkgd(split_line, COLOR_PAIR(3));
    wrefresh(split_line);
    wbkgd(top_line, COLOR_PAIR(3));
    wrefresh(top_line);
    wbkgd(bottom_line, COLOR_PAIR(3));
    wrefresh(bottom_line);
    wbkgd(left_column, COLOR_PAIR(3));
    wrefresh(left_column);
    wbkgd(right_column, COLOR_PAIR(3));
    wrefresh(right_column);

    initial_buffer_screen(display_height, size);

    /*wbkgd(display, COLOR_PAIR(1));*/
    /*wrefresh(display);*/
	/*show fix help information*/
	char *fix1 = "^G Get help    ^E Quit     ^Y Select    ^O Unselect    ^T Search key";
	char *fix2 = "^F Send File   ^W Latest   ^R Up page   ^D Down page   ^A Show contact";
	char *tmp = (char *)malloc(parent_x - 2);
	if (!tmp)
		return -1;

	memcpy(tmp, fix1, strlen(fix1) > (parent_x - 2) ? strlen(fix1) : (parent_x - 2));
	wprintw(display, "%s\n", tmp);
	memcpy(tmp, fix2, strlen(fix2) > (parent_x - 2) ? strlen(fix2) : (parent_x - 2));
	wprintw(display, "%s\n", tmp);
	wrefresh(display);

	return 0;
}

void show_base_info(WINDOW *display)
{
	if (!display)
		return;
	static bool flag_t = false;
//	static bool flag_h = false;

	if (!flag_t) {
		draw_new(display, "--------------------------------------------------");
		draw_new(display, "             Welcome to Free Chat!            ");
		draw_new(display, "--------------------------------------------------\n");
		flag_t = true;
	}

    draw_new(display, "**************************************************");
	draw_new(display, " ### FREECHAT HELP ###");
	draw_new(display, "^G Get this help info.");
	draw_new(display, "^E To exit from this program.");
	draw_new(display, "^Y Select a contact to chat with.");
	draw_new(display, "^O Get out from the contact current chating with.");
	draw_new(display, "^T Search the key word.");
	draw_new(display, "^F To send file to contact.");
	draw_new(display, "^W Goto the latest line.");
	draw_new(display, "^R Scroll screen up a page.");
	draw_new(display, "^D Scroll screen down a page.");
	draw_new(display, "^A To show all friends on line.");
    draw_new(display, "**************************************************\n");
}

WINDOW *get_display()
{
    WINDOW *display = newwin(parent_y - typing_size - 4 - 3, parent_x - 2, 4, 1);
    scrollok(display, 1);
	return display;
}

WINDOW *get_typing()
{
    WINDOW *input = newwin(typing_size, parent_x - 2, parent_y - typing_size - 2, 1);
	return input;
}

WINDOW *get_single_line()
{
    WINDOW *single_line = newwin(1, parent_x - 2, 3, 1);
	return single_line;
}

void cleanup_gui(WINDOW *display, WINDOW *input)
{
	if (!display || !input)
		return;

	draw_new(display, "\n**************************************************");
	draw_new(display, "             Good bye!!!");
	draw_new(display, "**************************************************\n");
	sleep(1);

	delwin(display);
	delwin(input);
	endwin();
}

