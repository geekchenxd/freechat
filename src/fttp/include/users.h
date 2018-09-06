#ifndef __USERS_H__
#define __USERS_H__
#include "fttp_default.h"
#include "debug.h"
#include "decode.h"
#include "apdu.h"
#include "npdu.h"

struct type_user {
	uint16_t id;
	char name[FTTPMAXNAMESIZE];
	char signature[FTTPMAXSIGNATURESIZE];
	char birthday[FTTPMAXBIRTHDAYSIZE];
	enum fttp_user_type sex;
};

void handler_user_req(uint8_t *data, uint16_t data_len,
		struct fttp_addr *src);
void handler_user_rsp(uint8_t *data, uint16_t data_len,
		struct fttp_addr *src);
void send_user_rsp(struct fttp_addr *dest, 
		struct type_user *me);
#endif
