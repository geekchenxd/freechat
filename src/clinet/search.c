#include "search.h"

extern struct node *root;
extern int line_amount;

int search(char *word, WINDOW *display){
    struct node *current;
    current = root;
	int hit=0;

    int current_line = line_amount - 2;
    int count = 0;
    int i, j;
    char buffer[LENLINE];

    draw_new(display, "system>> ### This /find have error value at 5 line. ###");
	while(current->next != 0 && count < current_line){
		current = current->next;
		if(strstr(current->string, word) != 0){
            sprintf(buffer, "system>> Found at line: %d", current->line_number);
			draw_new(display, buffer);
			draw_new(display, current->string);
			hit++;
		}
        count++;
	}
	if(hit==0){
		draw_new(display, "system>> Word not found.");
	}
	return -1;
}
