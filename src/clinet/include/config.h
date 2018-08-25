#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>
#include "client.h"

int config_parser(const char *path, struct info *info);

#endif
