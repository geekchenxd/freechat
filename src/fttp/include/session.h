#ifndef _SESSION_H__
#define _SESSION_H__
#include <stdint.h>
#include <stdbool.h>
#include "fttp_default.h"

#define MAX_FTTP_SESSION 255

struct fttp_session {
	uint8_t session_id;
	bool waiting;
	uint8_t retry;
	uint32_t timer;
	struct fttp_addr dest;
	uint8_t pdu[MAX_PDU];
	uint16_t pdu_len;
};

uint8_t fttp_free_session(void);
void fttp_session_set(uint8_t id, struct fttp_addr *dest,
		uint8_t *pdu, uint16_t pdu_len);
bool fttp_session_get_pdu(uint8_t id, struct fttp_addr *dest,
		uint8_t *pdu, uint16_t *pdu_len);
void fttp_session_process(uint16_t milliseconds);
void fttp_session_clear(uint8_t id);

#endif
