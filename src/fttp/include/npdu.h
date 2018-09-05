#ifndef _NPDU_H__
#define _NPDU_H__
#include <stdint.h>

struct fttp_npdu {
	uint8_t protocol_version;
};

int npdu_decode(uint8_t *npdu, struct fttp_npdu *npdu_data);
void npdu_handler(struct fttp_addr *src, uint8_t *pdu, 
		uint16_t pdu_len);
uint16_t encode_npdu(uint8_t *npdu);

#endif
