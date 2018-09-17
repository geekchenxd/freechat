#ifndef _DISPLAY_H__
#define _DISPLAY_H__

#include "client.h"

void *display_func(void *arg);
void msg_display(uint16_t user_id, uint8_t *text, uint16_t len);

#endif
