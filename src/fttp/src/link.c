#include <stdint.h>
#include "link.h"
#include "fttp_enum.h"
#include "decode.h"
#include "debug.h"
#include "fttp_udp.h"
#include "user_id.h"
#include "npdu.h"
#include "apdu.h"
#include "session.h"

void handler_link_test(uint8_t *data, uint16_t data_len,
		struct fttp_addr *src, uint8_t session_id)
{
	debug(INFO, "Recv Link Test from:%d.%d.%d.%d\n",
			src->addr[0], src->addr[1], src->addr[2], src->addr[3]);
	fttp_send_link_rsp(src, session_id);
}

void fttp_send_link_rsp(struct fttp_addr *dest, uint8_t session_id)
{
	uint8_t npdu[MAX_PDU];
	uint16_t pdu_len = 0;

	if (!dest)
		return;

	pdu_len = (uint16_t)fttp_encode_simple_ack(&npdu[0],
			FTTP_SERVICE_LINK_TEST, session_id);
	if (pdu_len > 0)
		fttp_send_udp(dest, &npdu[0], pdu_len);

	return;
}

int fttp_link_test(uint8_t user_id)
{
	struct fttp_addr dest = {0};
	uint8_t npdu[MAX_PDU];
	uint16_t len = 0;
	uint16_t pdu_len = 0;
	uint8_t session_id = 0;

	/*destination for text to transmit*/
	if(!fttp_addr_get_by_id(&dest, user_id)) {
		debug(DEBUG, "Can't find user by user id:%d\n", user_id);
		return 0;
	}

	len = encode_npdu(&npdu[pdu_len]);
	pdu_len += len;
	len = encode_apdu_common(&npdu[pdu_len], FTTP_PDU_REQ, 
			FTTP_SERVICE_LINK_TEST);
	pdu_len += len;

	/*encode the session id*/
	session_id = fttp_free_session();
	if (session_id) {
		npdu[pdu_len++] = session_id;
		if (pdu_len < MAX_PDU - 1) {
			fttp_session_set(session_id, &dest, &npdu[0], pdu_len);
		} else {
			fttp_session_clear(session_id);
			return 0;
		}
	} else {
		return session_id;
	}

	return (int)fttp_send_udp(&dest, &npdu[0], pdu_len);
}

