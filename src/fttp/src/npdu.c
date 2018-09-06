/*
 * We do not support more network layer function currently.
 */
#include "apdu.h"
#include "npdu.h"
#include "fttp_default.h"
#include "debug.h"

/*
 * decode the net work layer data of message. return zero on
 * error, or return the decoded length of net work layer data.
 */
int npdu_decode(uint8_t *npdu, struct fttp_npdu *npdu_data)
{
	if (!npdu || !npdu_data)
		return 0;

	npdu_data->protocol_version = npdu[0];

	return 1;
}

/*
 * handle the message of net work layer for fttp procotol
 * stack.
 */
void npdu_handler(struct fttp_addr *src, uint8_t *pdu, 
		uint16_t pdu_len)
{
	int decode_len = 0;
	struct fttp_npdu npdu_data = {0x0};

	if (pdu[0] != FTTP_PROTOCOL_VERSION)
		return;

	decode_len = npdu_decode(pdu, &npdu_data);
	debug(DEBUG, "the fttp protocol version is %d\n",
			npdu_data.protocol_version);
	if ((decode_len > 0) && (decode_len < pdu_len)) {
		pdu_len -= decode_len;
		apdu_handler(src, &pdu[decode_len], pdu_len);
	} else {
		debug(INFO, "fttp decode npdu error!\n");
	}
}

uint16_t encode_npdu(uint8_t *npdu)
{
	if (!npdu)
		return 0;

	npdu[0] = FTTP_PROTOCOL_VERSION;
	return 1;
}
