#ifndef _GUI_H__
#define _GUI_H__

int init_gui(uint8_t size);
WINDOW *get_display();
WINDOW *get_typing();
WINDOW *get_single_line();
void cleanup_gui(void);
void show_base_info(WINDOW *display);
bool cots();
void reinit_gui();

#endif
