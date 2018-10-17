#ifndef _HISTORY_H_
#define _HISTORY_H_
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ncurses.h>

/*max line length limite*/
#define LEN_LINE 500 


struct history {
	int line_number;
	char string[LEN_LINE];
	struct history *next;
};

uint32_t get_display_height();
void add_record(char *string);
void draw_new(WINDOW *display, char *string);
void screen_scroll(WINDOW *display, int dir, int lines);
void init_screen(int height_display, int height_typing);
int search(char *word, WINDOW *display);

#endif
