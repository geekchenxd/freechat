#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "connection_clent.h"
#include "c_string.h"
#include "client.h"
#include "gui.h"
#include "search.h"
#include "display.h"
#include "typing.h"



pthread_t *global_typing_thread, *global_display_thread;
int state = 0, display_height;
char username[LENGHT_USERNAME] = "";

struct client client;

int main(int argc , char *argv[]) {

    int read_size, port;
    char ip_address[255];
    char message_buffer[LENGHT_MESSAGE], c;
    char message_buffer_2[LENGHT_MESSAGE];
    WINDOW *buffer_window[2];
	struct client *p;

	p = &client;

	//the input area size
	p->gui.input_line = 4;
	if (init_gui(p->gui.input_line)) {
		fprintf(stderr, "init gui failed!\n");
		exit(1);
	}
	p->gui.display = get_display();
	p->gui.input = get_typing();
	show_base_info(p->gui.display);

    //prepare to pthread_create with WINDOW *buffer_window[2];
    pthread_t typing_thread, display_thread;
    global_typing_thread = &typing_thread;
    global_display_thread = &display_thread;
    pthread_create( &typing_thread, NULL, (void *)typing_func, (void *)p);
    pthread_create( &display_thread, NULL, (void *)display_func, (void *)p);

    pthread_join( typing_thread, NULL);
    pthread_join( display_thread, NULL);

	cleanup_gui(p->gui.display, p->gui.input);

    return 0;
}
