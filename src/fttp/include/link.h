#ifndef _LINK_H__
#define _LINK_H_
#include "fttp_default.h"

void handler_link_test(uint8_t *data, uint16_t data_len,
		struct fttp_addr *src, uint8_t session_id);
void fttp_send_link_rsp(struct fttp_addr *dest, uint8_t session_id);
int fttp_link_test(uint8_t user_id);


#endif
