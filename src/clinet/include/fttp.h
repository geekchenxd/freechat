#ifndef _FTTP_H_H_
#define _FTTP_H_H_

#include <stdint.h>
#include "client.h"

int send_text_udp(uint8_t *text, uint16_t text_len, 
		uint16_t user_id, uint16_t my_id);
bool fttp_init(char *ifname);
void fttp_task(struct client *p);

#endif
