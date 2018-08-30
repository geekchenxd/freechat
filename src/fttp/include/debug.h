#ifndef __DEBUG_H_
#define __DEBUG_H_

#include <stdio.h>

#define DEBUG_LEVEL 4
#define DEBUGING_ENABLED 1

#define ERROR 1
#define WARNING 2
#define INFO 3
#define DEBUG 4

#define debug(level,fmt, args...) \
	(level<=DEBUG_LEVEL) ? printf(fmt, ## args) : level

#endif
