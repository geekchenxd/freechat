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

int fttp_user_rsp_decode(uint8_t *data, uint16_t data_len,
		struct type_user *user);
void handler_user_req(uint8_t *data, uint16_t data_len,
		struct fttp_addr *src);
void handler_user_rsp(uint8_t *data, uint16_t data_len,
		struct fttp_addr *src);
void send_user_rsp(struct fttp_addr *dest, 
		struct type_user *me);
void send_user_req();
void *user_id_consult(void *arg);
#endif
