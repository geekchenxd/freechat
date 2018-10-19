#include "client.h"
#include <locale.h>

/*
 * the mini lines is the sum of top line, fix lines,
 * hide line, display lines, split line, typing lines
 * and a bottom line
 */
#define MINILINES 1+2+1+4+1+4+1

/*
 * the mini columns is the the length the longest fix 
 * info is;
 */
#define MINICOLUMNS 72

static int parent_x, parent_y;
int typing_size;
static WINDOW *top_line = NULL;
static WINDOW *bottom_line = NULL;
static WINDOW *left_column = NULL;
static WINDOW *right_column = NULL;
static WINDOW *fix_display = NULL;
static WINDOW *split_line = NULL;
static WINDOW *display = NULL;
static WINDOW *input = NULL;
static WINDOW *single_line = NULL;

int init_gui(uint8_t size)
{
	if (size <= 0)
		return -2;

	int display_height;
	typing_size = size;

	/*use the system's locale to support Chinese*/
	setlocale(LC_ALL, "");
    initscr();

	/*set cursor Invisible*/
	curs_set(0);

    start_color();
    init_pair(1, COLOR_BLACK, COLOR_GREEN);
    init_pair(2, COLOR_BLACK, COLOR_RED);
    init_pair(3, COLOR_BLACK, COLOR_WHITE);

    /* get our maximum window dimensions */
    getmaxyx(stdscr, parent_y, parent_x);
	if ((parent_y < MINILINES) || (parent_x < MINICOLUMNS)) {
		endwin();
		printf("Your screen is too small to run freechat!!!\n");
		printf("It needs at lease %d lines and and %d columns!\n",
				MINILINES, MINICOLUMNS);
		return -1;
	}

    /* draw windows */
    top_line = newwin(1, parent_x, 0, 0);
    bottom_line = newwin(1, parent_x, parent_y - 1, 0);
    left_column = newwin(parent_y - 2, 1, 1, 0);
    right_column = newwin(parent_y - 2, 1, 1, parent_x - 1);
    fix_display = newwin(2, parent_x - 2, 1, 1);
    split_line = newwin(1, parent_x - 2, parent_y - size - 3, 1);

    display_height = parent_y - size - 3; /*top and bottom line and split line*/
	display_height -= 3; /*fix info area and hide line*/

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

    init_screen(display_height, size);

	/*show fix help information*/
	char *fix1 = "^G Get help    ^E Quit     ^Y Select    ^O Unselect    ^T Search key";
	char *fix2 = "^F Send File   ^W Latest   ^R Up page   ^D Down page   ^A Show contact";
	char *tmp = (char *)malloc(parent_x - 2);
	if (!tmp)
		return -1;

	memcpy(tmp, fix1, strlen(fix1) > (parent_x - 2) ? strlen(fix1) : (parent_x - 2));
	wprintw(fix_display, "%s\n", tmp);
	memcpy(tmp, fix2, strlen(fix2) > (parent_x - 2) ? strlen(fix2) : (parent_x - 2));
	wprintw(fix_display, "%s\n", tmp);
	wrefresh(fix_display);
	free(tmp);

	return 0;
}

void show_base_info(WINDOW *display)
{
	if (!display)
		return;
	static bool flag_h = false;
	static bool flag_t = false;

	/*
	 * display this head info only on initialization
	 */
	if (!flag_h) {
		draw_new(display, "--------------------------------------------------");
		draw_new(display, "             Welcome to Free Chat!            ");
		draw_new(display, "--------------------------------------------------\n");
		flag_h = true;
	}

    draw_new(display, "**************************************************");
	draw_new(display, " ### FREECHAT HELP ###");
	draw_new(display, "^G Get detail help info.");
	draw_new(display, "^E To exit from this program.");
	draw_new(display, "^Y Select a contact to chat with.");
	draw_new(display, "^O Get out from the contact current chating with.");
	draw_new(display, "^T Search the key word.");
	draw_new(display, "^F To send file to contact.");
	draw_new(display, "^W Goto the latest line.");
	draw_new(display, "^R Scroll screen up a page.");
	draw_new(display, "^D Scroll screen down a page.");
	draw_new(display, "^A To show all friends on line.");
	if (!flag_t) {
		draw_new(display, "**************************************************");
	}

	/*
	 * this help info not display during initialization
	 */
	if (flag_t) {
		draw_new(display, "Advanced Options:");
		draw_new(display, ":server    Set default server info.");
		draw_new(display, ":info      Display user details.");
		draw_new(display, ":edit      Edit my info.");
		draw_new(display, ":refresh   Refresh user list.");
		draw_new(display, ":log       Save chat log.");
		draw_new(display, ":lock      Disable users' message.");
		draw_new(display, ":locked    Display disabled user list.");
		draw_new(display, ":unlock    Enable users' message.");
		draw_new(display, ":login     Log in the server.");
		draw_new(display, ":logout    Log out from the server.");
		draw_new(display, ":register  Register to server.");
		draw_new(display, ":unreg     Unregister from server.");
		draw_new(display, ":create    Create a room.");
		draw_new(display, ":destroy   Destroy room");
		draw_new(display, ":bcast     Broadcast messages.");
		draw_new(display, "**************************************************");
	}
	flag_t = true;
}

WINDOW *get_display()
{
    display = newwin(parent_y - typing_size - 4 - 3, parent_x - 2, 4, 1);
    scrollok(display, 1);
	return display;
}

WINDOW *get_typing()
{
    input = newwin(typing_size, parent_x - 2, parent_y - typing_size - 2, 1);
	return input;
}

WINDOW *get_single_line()
{
    single_line = newwin(1, parent_x - 2, 3, 1);
	return single_line;
}

/*
 * event for change of terminal size.
 */
bool cots()
{
	bool changed = false;
	int y = 0;
	int x = 0;
    getmaxyx(stdscr, y, x);
	if (x != parent_x) {
		changed = true;
		parent_x = x;
	}
	if (y != parent_y) {
		changed = true;
		parent_y = y;
	}

	return changed;
}

void delete_win()
{
    wbkgd(split_line, COLOR_PAIR(4));
    wrefresh(split_line);
    wbkgd(top_line, COLOR_PAIR(4));
    wrefresh(top_line);
    wbkgd(bottom_line, COLOR_PAIR(4));
    wrefresh(bottom_line);
    wbkgd(left_column, COLOR_PAIR(4));
    wrefresh(left_column);
    wbkgd(right_column, COLOR_PAIR(4));
    wrefresh(right_column);
	delwin(top_line);
	delwin(bottom_line);
	delwin(left_column);
	delwin(right_column);
	delwin(fix_display);
	delwin(split_line);
	delwin(display);
	delwin(input);
	delwin(single_line);
}


void reinit_gui()
{
	int display_height = 0;

    display_height = parent_y - typing_size - 3; /*top and bottom line and split line*/
	display_height -= 3; /*fix info area and hide line*/
	
	delete_win();

	if ((parent_y < MINILINES) || (parent_x < MINICOLUMNS)) {
		printw("Your screen is too small to run freechat!!!\n");
		printw("It needs at lease %d lines and and %d columns!\n",
				MINILINES, MINICOLUMNS);
		return;
	}
    /* draw windows */
    top_line = newwin(1, parent_x, 0, 0);
    bottom_line = newwin(1, parent_x, parent_y - 1, 0);
    left_column = newwin(parent_y - 2, 1, 1, 0);
    right_column = newwin(parent_y - 2, 1, 1, parent_x - 1);
    fix_display = newwin(2, parent_x - 2, 1, 1);
    split_line = newwin(1, parent_x - 2, parent_y - typing_size - 3, 1);


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

    init_screen(display_height, typing_size);

	/*show fix help information*/
	char *fix1 = "^G Get help    ^E Quit     ^Y Select    ^O Unselect    ^T Search key";
	char *fix2 = "^F Send File   ^W Latest   ^R Up page   ^D Down page   ^A Show contact";
	char *tmp = (char *)malloc(parent_x - 2);
	if (!tmp)
		return;

	memcpy(tmp, fix1, strlen(fix1) > (parent_x - 2) ? strlen(fix1) : (parent_x - 2));
	wprintw(fix_display, "%s\n", tmp);
	memcpy(tmp, fix2, strlen(fix2) > (parent_x - 2) ? strlen(fix2) : (parent_x - 2));
	wprintw(fix_display, "%s\n", tmp);
	wrefresh(fix_display);
	free(tmp);
}

void cleanup_gui(void)
{
	draw_new(display, "\n**************************************************");
	draw_new(display, "             Good bye!!!");
	draw_new(display, "**************************************************\n");
	sleep(1);
	delete_win();
	endwin();
}

