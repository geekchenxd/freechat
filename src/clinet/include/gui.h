#ifndef _GUI_H__
#define _GUI_H__

int init_gui(uint8_t size);
WINDOW *get_display();
WINDOW *get_typing();
WINDOW *get_single_line();
void show_base_info(WINDOW *display);
void cleanup_gui(WINDOW *display, WINDOW *input);

#endif
