#ifndef __TEXT_H__
#define __TEXT_H__
#include <stdint.h>
#include "fttp_default.h"

void handler_trans_text(uint8_t *data, uint16_t data_len,
		struct fttp_addr *src, uint8_t session_id);
int fttp_trans_text(uint8_t *text, uint16_t text_len, 
		uint8_t user_id, uint16_t my_id);
int fttp_text_decode(uint8_t *data, uint16_t data_len,
		uint8_t *text);

#endif
