#ifndef _FTTP_HANDLE_H__
#define _FTTP_HANDLE_H__
#include <stdint.h>

void freechat_handler_user_req(uint8_t *data, uint16_t data_len,
		struct fttp_addr *src);
void freechat_handler_user_rsp(uint8_t *data, uint16_t data_len,
		struct fttp_addr *src);
void freechat_handler_trans_text(uint8_t *data, uint16_t data_len,
		struct fttp_addr *src, uint8_t session_id);
void freechat_handle_session_failed(uint8_t id);

#endif
