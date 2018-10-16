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
int update_sex_config(uint8_t sex, char *configfile);
int update_birthday_config(char *birthday, char *configfile);
int update_signature_config(char *signature, char *configfile);
int update_name_config(char *name, char *configfile);

#endif
