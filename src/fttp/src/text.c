#include "text.h"
#include "debug.h"
#include "decode.h"
#include "fttp_udp.h"
#include "session.h"
#include "npdu.h"
#include "apdu.h"
#include "user_id.h"

int fttp_text_decode(uint8_t *data, uint16_t data_len,
		uint8_t *text)
{
	int len = 0;

	len = fttp_decode_string(data, (char *)text);
	if (len > data_len)
		return 0;
	if (len > 0) {
		text[len] = '\0';
	}

	return len;
}

void handler_trans_text(uint8_t *data, uint16_t data_len,
		struct fttp_addr *src, uint8_t session_id)
{
	uint8_t text[FTTP_MAX_TEXT_SIZE];
	uint16_t decode_len = 0;
	uint16_t encode_len = 0;
	int len = 0;
	uint16_t user_id = 0;
	
	/*
	 * the data[0] is the user's id
	 */
	len = fttp_decode_id(&data[0], &user_id);
	decode_len += len;

	debug(DEBUG, "Recv Text From IP:%d.%d.%d.%d\n", src->addr[0],
			src->addr[1], src->addr[2], src->addr[3]);
	debug(DEBUG, "User ID is %d\n", user_id);

	len = fttp_text_decode(&data[decode_len], data_len - decode_len, &text[0]);
	decode_len += len;

	if (decode_len != data_len) {
		debug(ERROR, "Decode fttp text failed!\n");
		perror("");
		return;
	}
	/*handle the text*/
	debug(INFO, "[%s]\n", text); 

	/*send a simple ack*/
	len = fttp_encode_simple_ack(&text[0], session_id);
	encode_len += len;
	if (encode_len > 0) {
		fttp_send_udp(src, &text[0], encode_len);
	}
}

int fttp_trans_text(uint8_t *text, uint16_t text_len, 
		uint8_t user_id, uint16_t my_id)
{
	struct fttp_addr dest;
	uint8_t npdu[MAX_PDU];
	uint16_t len = 0;
	uint16_t pdu_len = 0;
	uint8_t session_id = 0;

	if (!text || !text_len)
		return 0;

	/*destination for text to transmit*/
	if(!fttp_addr_get_by_id(&dest, user_id)) {
		debug(DEBUG, "Can't find user by user id:%d\n", user_id);
		return 0;
	}

	len = encode_npdu(&npdu[pdu_len]);
	pdu_len += len;
	len = encode_apdu_common(&npdu[pdu_len], FTTP_PDU_REQ, 
			FTTP_SERVICE_TRANS_TEXT);
	pdu_len += len;

	/*encode the session id*/
	session_id = fttp_free_session();
	if (session_id) {
		npdu[pdu_len++] = session_id;
		len = fttp_encode_id(&npdu[pdu_len], my_id);
		pdu_len += len;

		len = fttp_encode_string(&npdu[pdu_len], text, text_len);
		pdu_len += len;

		if (pdu_len < MAX_PDU - 1) {
			fttp_session_set(session_id, &dest, &npdu[0], pdu_len);
		} else {
			fttp_session_clear(session_id);
			return 0;
		}
	}

	return (int)fttp_send_udp(&dest, &npdu[0], pdu_len);
}
	
