#ifndef _APDU_H__
#define _APDU_H__

#include <stdint.h>
#include "users.h"
#include "test.h"
#include "link.h"
#include "file.h"
#include "fttp_enum.h"
#include "fttp_default.h"


uint8_t apdu_retry(void);
void apdu_retry_set(uint8_t count);
uint32_t apdu_timeout(void);
void apdu_timeout_set(uint32_t timeout);
void apdu_service_init(void);
void apdu_handler(struct fttp_addr *src,
		uint8_t *apdu, uint16_t pdu_len);



#endif
