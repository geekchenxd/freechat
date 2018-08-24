#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <langinfo.h>
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

void disable_signals(void)
{
	struct termios term = {0};

	tcgetattr(0, &term);
	term.c_lflag &= ~ISIG;
	tcsetattr(0, TCSANOW, &term);
}

void disable_extended_io(void)
{
	struct termios term = {0};

	tcgetattr(0, &term);
	term.c_lflag &= ~IEXTEN;
	term.c_oflag &= ~OPOST;
	tcsetattr(0, TCSANOW, &term);
}

void terminal_init(void)
{
	raw();
	nonl();
	noecho();
}


void process_exit(int signal_num)
{
    pthread_cancel(*global_display_thread);
    pthread_cancel(*global_typing_thread);
	cleanup_gui(client.gui.display, client.gui.input);
	exit(0);
}

int main(int argc , char *argv[]) 
{
    //char message_buffer[LENGHT_MESSAGE], c;
    //char message_buffer_2[LENGHT_MESSAGE];
    //WINDOW *buffer_window[2];
	struct sigaction sig;
	struct client *p;

	p = &client;

	/* signal handler */
	sig.sa_handler = process_exit;
	sigemptyset(&sig.sa_mask);
	sig.sa_flags = 0;
	sigaction(SIGINT, &sig, NULL);

	terminal_init();

	/* init graphyics */
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
    pthread_create(&typing_thread, NULL, (void *)typing_func, (void *)p);
    pthread_create(&display_thread, NULL, (void *)display_func, (void *)p);

    pthread_join( typing_thread, NULL);
    pthread_join( display_thread, NULL);

	cleanup_gui(p->gui.display, p->gui.input);

    return 0;
}
