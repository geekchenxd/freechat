#ifndef _FTTP_TCP_H_
#define _FTTP_TCP_H_

#include <stdint.h>

int fttp_recieve_tcp(
		uint8_t *pdu, uint16_t max_pdu, int socket);
int fttp_send_tcp(uint8_t *pdu, uint16_t pdu_len, int socket);
#endif
