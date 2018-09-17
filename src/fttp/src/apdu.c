#include "apdu.h"
#include "session.h"

/*
 * apdu timeout indicate the longest time that a session wait for reply.
 */
static uint32_t fttp_apdu_timeout = 3000;
/*
 * apdu_retry indicate once the session waiting reply timeout, how many
 * times the request of this session will be retried.
 */
static uint8_t fttp_apdu_retry = 1;

/*
 * messages hanler function.
 */
static void (*trans_file_handle)(uint8_t *, uint16_t, struct fttp_addr *, uint8_t);
static void (*trans_text_handle)(uint8_t *, uint16_t, struct fttp_addr *, uint8_t);
static void (*test_handle)(uint8_t *, uint16_t, struct fttp_addr *, uint8_t);
static void (*user_req_handle)(uint8_t *, uint16_t, struct fttp_addr *);
static void (*user_rsp_handle)(uint8_t *, uint16_t, struct fttp_addr *);
//static void (*error_handle)(uint8_t, uint16_t, struct fttp_addr *, uint8_t);


uint8_t apdu_retry(void)
{
	return fttp_apdu_retry;
}

void apdu_retry_set(uint8_t count)
{
	fttp_apdu_retry = count;
}

uint32_t apdu_timeout(void)
{
	return fttp_apdu_timeout;
}

void apdu_timeout_set(uint32_t timeout)
{
	fttp_apdu_timeout = timeout;
}

/*
 * initialeze the message handler functions.
 * here is the default set.
 */
void apdu_service_init(void)
{
	trans_file_handle = handler_trans_file;
	trans_text_handle = handler_trans_text;
	user_req_handle = handler_user_req;
	user_rsp_handle = handler_user_rsp;
	//error_handle = hanler_error;
	//test_handle = handler_test;
}

/*
 * set the apdu handlers.
 */
void set_trans_file_handle(
		void (*fun)(uint8_t*,uint16_t, struct fttp_addr*, uint8_t))
{
	if (fun)
		trans_file_handle = fun;
}

void set_trans_text_handle(
		void (*fun)(uint8_t*,uint16_t, struct fttp_addr*, uint8_t))
{
	if (fun)
		trans_text_handle = fun;
}

void set_test_handle(
		void (*fun)(uint8_t*,uint16_t, struct fttp_addr*, uint8_t))
{
	if (fun)
		test_handle = fun;
}

void set_user_req_handle(
		void (*fun)(uint8_t*,uint16_t, struct fttp_addr*))
{
	if (fun)
		user_req_handle = fun;
}

void set_user_rsp_handle(
		void (*fun)(uint8_t*,uint16_t, struct fttp_addr*))
{
	if (fun)
		user_rsp_handle = fun;
}

/*
 * according to diffrent pdu type and service call the 
 * service handler function.
 */
void apdu_handler(struct fttp_addr *src, 
		uint8_t *apdu, uint16_t pdu_len)
{
	uint8_t session_id;
	enum fttp_service service;
	uint16_t decode_len = 0;

	if (!apdu)
		return;
	if ((apdu[0] >= FTTP_PDU_TYPE_MAX) ||
			(apdu[1] >= FTTP_SERVICE_MAX))
		return;
	service = apdu[1];
	session_id = apdu[2];
	decode_len += 3;

	switch (apdu[0]) {	/* pdu type */
	case FTTP_PDU_REQ:
		switch (service) {
		case FTTP_SERVICE_TRANS_TEXT:
			if (trans_text_handle)
				trans_text_handle(&apdu[decode_len],
						pdu_len - decode_len, src, session_id);
			 break;
		case FTTP_SERVICE_TRANS_FILE:
			if (trans_file_handle)
				trans_file_handle(&apdu[decode_len],
						pdu_len - decode_len, src, session_id);
			 break;
		case FTTP_SERVICE_LINK_TEST:
			if (test_handle)
				test_handle(&apdu[decode_len],
						pdu_len - decode_len, src, session_id);
			 break;
		case FTTP_SERVICE_USER_REQ:
			decode_len -= 1; /*no session id */
			if (user_req_handle)
				user_req_handle(&apdu[decode_len],
						pdu_len - decode_len, src);
			 break;
		default:
			 break;
		}
		break;
	case FTTP_PDU_RSP_SIMPLE:
		debug(DEBUG, "the simple ack's session_id is %d\n", session_id);
		fttp_session_clear(session_id);
		break;
	case FTTP_PDU_RSP_COMPLEX:
		if (service == FTTP_SERVICE_USER_RSP) {
			decode_len -= 1;   /*no session id*/
			if (user_rsp_handle)
				user_rsp_handle(&apdu[decode_len],
						pdu_len - decode_len, src);
		}
		break;
	case FTTP_PDU_ERROR:
		fttp_session_clear(session_id);
		break;
	}
}

uint16_t encode_apdu_common(uint8_t *apdu, enum pdu_type pdu_type,
		enum fttp_service service)
{
	if (!apdu)
		return 0;

	apdu[0] = (uint8_t)pdu_type;
	apdu[1] = (uint8_t)service;

	return 2;
}
