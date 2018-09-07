#ifndef __FILE_H_
#define __FILE_H_
#include "fttp_default.h"

void handler_trans_file(uint8_t *data, uint16_t data_len,
		struct fttp_addr *src, uint8_t session_id);


#endif
