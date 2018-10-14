#ifndef _CLIENT_H_
#define _CLIENT_H_
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include "history.h"
#include "user_list.h"

#define LENGHT_USERNAME 255
#define LENGHT_MESSAGE 255
#define CONFIGFILEPATH "/etc/freechat/freechat.cfg"

struct gui {
	WINDOW *display;
	WINDOW *input;
	WINDOW *single_line;
	uint8_t input_line;
};

struct info {
	uint8_t serverip[16];
	uint16_t serverport;
	uint16_t local_port;
	uint16_t port_local;
};

struct client {
	struct gui gui;
	struct info info;
	struct user_list *user;
	char *ifname;
	char *cfg_path;
};

extern struct client client;

#endif
