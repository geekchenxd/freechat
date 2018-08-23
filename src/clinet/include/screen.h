#ifndef _SCREEN_H_
#define _SCREEN_H_
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#define LENLINE 500 //define limit lenght of each one line

//global variable assignment
struct node {
	int line_number;
	char string[LENLINE];
	struct node *next; //next and previous address node linked
};

uint32_t get_display_height();
void add_line(char *string);
void draw_new(WINDOW *display, char *string);
void draw_old_line(WINDOW *display, int option, int line_user_want_scroll);

void initial_buffer_screen(int dis_h, int ty_h);

#endif
