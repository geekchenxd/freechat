#ifndef __GUI_H__
#define __GUI_H__

#include <locale.h>

struct gui {
	WINDOW *display; /* main display area */
	WINDOW *dis_fix; /* fix info display area */
	WINDOW *type; /* typing area */
	WINDOW *split; /* split line */
	WINDOW *top_line;
	WINDOW *bot_line;
	WINDOW *left_col;
	WINDOW *right_col;
	WINDOW *singline; /* single line display area */
	int type_height; /* type area height */
	int dis_height; /* display area height */
	int parent_x; /* current screen width */
	int parent_y; /* current screed height */

	/* type function list */
	/* display function list */
	void (*put_raw)(struct gui *, char *);
	void (*put_raw_fix)(struct gui *, char *);
	void (*put_raw_single)(struct gui *, char *);
	void (*upward)(struct gui *, int);
	void (*downward)(struct gui *, int);
};

#endif
