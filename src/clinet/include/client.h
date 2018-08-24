#ifndef _CLIENT_H_
#define _CLIENT_H_
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include "screen.h"
#include "user_list.h"

#define LENGHT_USERNAME 255
#define LENGHT_MESSAGE 255

struct gui {
	WINDOW *display;
	WINDOW *input;
	uint8_t input_line;
};

struct info {
	uint8_t serverip[16];
	uint16_t serverport;
	uint16_t local_port;
};

struct client {
	struct gui gui;
	struct info info;
	struct user_list *user;
};

#endif
