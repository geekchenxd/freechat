#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "users.h"
#include "debug.h"
#include "user_id.h"
#include "text.h"

static uint16_t g_id = 0;

int fttp_user_rsp_decode(uint8_t *data, uint16_t data_len,
		struct type_user *user)
{
	int len = 0;
	int decode_len = 0;

	if (!data || !data_len || !user)
		return 0;

	len = fttp_decode_id(&data[decode_len], &user->id);
	if (len <= 0)
		return 0;
	decode_len += len;
	len = fttp_decode_string(&data[decode_len], user->name);
	if (len <= 0)
		return 0;
	decode_len += len;
	len = fttp_decode_string(&data[decode_len], user->signature);
	if (len <= 0)
		return 0;
	decode_len += len;
	len = fttp_decode_string(&data[decode_len], user->birthday);
	if (len <= 0)
		return 0;
	decode_len += len;
	len = fttp_decode_enum(&data[decode_len], (uint8_t *)&user->sex);
	if (len <= 0)
		return 0;
	decode_len += len;

	return decode_len;
}

void handler_user_req(uint8_t *data, uint16_t data_len,
		struct fttp_addr *src)
{
	debug(INFO, "Recv User request from:%d.%d.%d.%d\n", 
			src->addr[0], src->addr[1], src->addr[2], src->addr[3]);
	struct type_user me;
	if (g_id == 0)
		g_id = fttp_gen_id();
	me.id = g_id;
	memcpy(&me.name[0], "chenxiaodong", sizeof("chenxiaodong"));
	memcpy(&me.signature[0], "no zuo no die", sizeof("no zuo no die"));
	memcpy(&me.birthday[0], "1994-05-24", sizeof("1994-05-24"));
	me.sex = 1;
	send_user_rsp(src, &me);
}

void handler_user_rsp(uint8_t *data, uint16_t data_len,
		struct fttp_addr *src)
{
	struct type_user *new;
	uint16_t decode_len = 0;
	int len = 0;

	if (!data  || !data_len)
		return;

	new = (struct type_user *)malloc(sizeof(struct type_user));
	if (!new) {
		debug(ERROR, "allocate the new user failed!\n");
		return;
	}

	len += fttp_user_rsp_decode(data, data_len, new);
	decode_len += len;
	if (decode_len != data_len) {
		debug(ERROR, "decode user response failed!\n");
		free(new);
		new = NULL;
		return;
	}
	/*here to handle the user info*/
	debug(INFO, "receive a user rsp From ID:%u IP:%d.%d.%d.%d\n",
			new->id, src->addr[0], src->addr[1], src->addr[2], src->addr[3]);
	debug(INFO, "name:%s\n", new->name);
	debug(INFO, "signature:%s\n", new->signature);
	debug(INFO, "birthday:%s\n", new->birthday);
	if (new->sex == FTTP_USER_ROOM)
		debug(INFO, "this user is a chating room!\n");
	else
		debug(INFO, "sex:%s\n",
				(new->sex == FTTP_USER_BOY) ? "boy" : "girl");

	if (!fttp_user_id_add(src, new->id)) {
		debug(ERROR, "add new user to id cache failed!\n");
	}
	/*release resource*/
	free(new);
	new = NULL;
}

void send_user_rsp(struct fttp_addr *dest, 
		struct type_user *me)
{
	struct fttp_addr tmp;
	uint8_t npdu[MAX_PDU];
	uint16_t len = 0;
	uint16_t pdu_len = 0;
	

	if (!dest)
		fttp_get_broadcast_address(&tmp);
	else
		memcpy(&tmp, dest, sizeof(struct fttp_addr));
	
	len = encode_npdu(&npdu[pdu_len]);
	pdu_len += len;
	len = encode_apdu_common(&npdu[pdu_len], FTTP_PDU_RSP_COMPLEX, 
			FTTP_SERVICE_USER_RSP);
	pdu_len += len;
	
	len = fttp_encode_id(&npdu[pdu_len], me->id);
	pdu_len += len;

	len = fttp_encode_string(&npdu[pdu_len], (uint8_t *)me->name, 
			(uint32_t)strlen(me->name));
	pdu_len += len;

	len = fttp_encode_string(&npdu[pdu_len], (uint8_t *)me->signature, 
			(uint32_t)strlen(me->signature));
	pdu_len += len;

	len = fttp_encode_string(&npdu[pdu_len], (uint8_t *)me->birthday, 
			(uint32_t)sizeof(me->birthday));
	pdu_len += len;

	len = fttp_encode_enum(&npdu[pdu_len], (uint8_t)me->sex);
	pdu_len += len;

	fttp_send_udp(&tmp, &npdu[0], pdu_len);
}

void send_user_req()
{
	uint16_t pdu_len = 0;
	uint16_t len = 0;
	uint8_t npdu[MAX_PDU];
	struct fttp_addr dest;

	fttp_get_broadcast_address(&dest);
	len = encode_npdu(&npdu[pdu_len]);
	pdu_len += len;
	len = encode_apdu_common(&npdu[pdu_len], FTTP_PDU_REQ, 
			FTTP_SERVICE_USER_REQ);
	pdu_len += len;
	debug(DEBUG, "Send user request to network! len = %d\n", pdu_len);
	fttp_send_udp(&dest, &npdu[0], pdu_len);
}

void *user_id_consult(void *arg)
{
	//uint16_t user_id = 0;
	struct type_user me;
	uint8_t opt = 0;
	opt = *((uint8_t *)arg);

	send_user_req();
	fttp_id_gen_wait(0);
#if 0
	user_id = fttp_gen_id();
	g_id = user_id;
#endif
	if(g_id == 0)
		g_id = fttp_gen_id();

	me.id = g_id;
	memcpy(&me.name[0], "chenxiaodong", sizeof("chenxiaodong"));
	memcpy(&me.signature[0], "no zuo no die", sizeof("no zuo no die"));
	memcpy(&me.birthday[0], "1994-05-24", sizeof("1994-05-24"));
	me.sex = 1;
	send_user_rsp(NULL, &me);

	if (opt == 0) {
		while (1) {
			fttp_trans_text((uint8_t *)"hello world", sizeof("hello world"), 1, me.id);
			fttp_id_gen_wait(0);
		}
	}
	return NULL;
}

