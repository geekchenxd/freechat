#ifndef _APDU_H__
#define _APDU_H__

#include <stdint.h>
#include "users.h"
#include "text.h"
#include "link.h"
#include "file.h"
#include "fttp_enum.h"
#include "fttp_default.h"
#include "fttp_udp.h"


uint8_t apdu_retry(void);
void apdu_retry_set(uint8_t count);
uint32_t apdu_timeout(void);
void apdu_timeout_set(uint32_t timeout);
void apdu_service_init(void);
void apdu_handler(struct fttp_addr *src,
		uint8_t *apdu, uint16_t pdu_len);
uint16_t encode_apdu_common(uint8_t *apdu, enum pdu_type pdu_type,
		enum fttp_service service);
void set_trans_file_handle(
		void (*fun)(uint8_t*,uint16_t, struct fttp_addr*, uint8_t));
void set_trans_text_handle(
		void (*fun)(uint8_t*,uint16_t, struct fttp_addr*, uint8_t));
void set_test_handle(
		void (*fun)(uint8_t*,uint16_t, struct fttp_addr*, uint8_t));
void set_user_req_handle(
		void (*fun)(uint8_t*,uint16_t, struct fttp_addr*));
void set_user_rsp_handle(
		void (*fun)(uint8_t*,uint16_t, struct fttp_addr*));



#endif
