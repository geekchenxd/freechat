#include "search.h"

extern struct node *root;
extern int line_amount;

int search(char *word, WINDOW *display){
    struct node *current;
    current = root;
	int hit=0;

    int current_line = line_amount - 2;
    int count = 0;
    char buffer[LEN_LINE];

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
	if(hit==0)
		return -1;

	return 0;
}
