#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>
#include "client.h"

extern char ethname[20];

int config_parser(const char *path, struct client *client);
int update_serverport_config(uint16_t port, char *configfile);
int update_serverip_config(char *ip, char *configfile);

#endif
