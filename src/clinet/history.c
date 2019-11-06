#include "history.h"

struct history *current, *root;
int line_amount = 0, g_bottom_line = 0;
int display_height, typing_height;


/*return the display height*/
uint32_t get_display_height()
{
	return display_height;
}


/*
 * set the display screen and typing height
 * init the history link.
 */
void init_screen(int height_display, int height_typing)
{

	display_height = height_display;
	typing_height = height_typing;

	/* Initial line list */
	root = malloc(sizeof(struct history));
	root->next = NULL;
	current = root;
}


/*
 * add a record to history link struct
 */
void add_record(char *string)
{
	while(current->next != 0) 
		current = current->next;

	current->next = malloc(sizeof(struct history));
	current = current->next;
	current->next = NULL;

	strcpy(current->string, string);

	current->line_number = line_amount;
}

/*
 * draw a new message string on display screen,
 * this function will add the string to the global
 * history link before draw it.
 */
void draw_new(WINDOW *display, char *string) 
{

	int i;

	if (g_bottom_line < line_amount) {
		screen_scroll(display, 1, line_amount - (g_bottom_line - 1));
	}

	if (!string)
		return;

	/*record*/
	add_record(string);

	/*print string*/
	line_amount++;
	for (i=0;i<strlen(string);i++) {
		if (string[i] == '\n') 
			line_amount += 1;
	}

	g_bottom_line = line_amount;

	wprintw(display, "%s\n", string);
	wrefresh(display);
}

/*
 * scroll screen up or down.
 * dir: direction for scroll, 0 for up, 1 for down
 * lines: lines counts we want to scroll
 */
void screen_scroll(WINDOW *display, int dir, int lines)
{

	int desc_line, i;

	for (i=0;i<lines;i++) {
		if (dir == 0) /*up*/
			desc_line = g_bottom_line - display_height;
		else	/*down*/
		   	desc_line = g_bottom_line;

		if (desc_line <= 0)
			break;
		current = root;
		while (current->next && current->next->line_number < desc_line) 
			current = current->next;


		if (dir == 0 && g_bottom_line > display_height){
			wscrl(display, -1);
			mvwprintw(display, 0, 0, "%s\n", current->string);
			g_bottom_line--;
		} else if (dir == 1 && g_bottom_line <= line_amount) {
			wscrl(display, 0);
			wmove(display, display_height - 2, 0);
			wprintw(display, "%s\n", current->string);
			g_bottom_line++;
		}
	}

	wrefresh(display);
}

/*
 * search string in history
 */
int search(char *word, WINDOW *display)
{
    struct history *tmp;
    tmp = root;
	int hit=0;

    int current_line = line_amount - 2;
    int count = 0;
    char buffer[LEN_LINE];

	while(tmp->next && count < current_line){
		tmp = tmp->next;
		if(strstr(tmp->string, word) != 0){
            sprintf(buffer, "system>> Found at line: %d", tmp->line_number);
			draw_new(display, buffer);
			draw_new(display, tmp->string);
			hit++;
		}
        count++;
	}
	if(hit==0)
		return -1;

	return 0;
}
