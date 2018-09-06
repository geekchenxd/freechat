#ifndef __TEXT_H__
#define __TEXT_H__
#include <stdint.h>
#include "fttp_default.h"

void handler_trans_text(uint8_t *data, uint16_t data_len,
		struct fttp_addr *src, uint8_t session_id);

#endif
