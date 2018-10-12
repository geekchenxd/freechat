#ifndef _HISTORY_H_
#define _HISTORY_H_
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ncurses.h>

/*max line length limite*/
#define LEN_LINE 500 


struct node {
	int line_number;
	char string[LEN_LINE];
	struct node *next;
};

uint32_t get_display_height();
void add_record(char *string);
void draw_new(WINDOW *display, char *string);
void draw_old_line(WINDOW *display, int option, int line_user_want_scroll);

void initial_buffer_screen(int dis_h, int ty_h);

#endif
