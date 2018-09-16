#include <stdio.h>
#include "client.h"
#include "users.h"
#include "user_id.h"

extern struct client client;

void freechat_handler_user_req(uint8_t *data, uint16_t data_len,
		struct fttp_addr *src)
{
	struct type_user *me = NULL;

	me = client.user->user;
	if (me->id == 0)
		me->id = fttp_gen_id();
	send_user_rsp(src, me);
}

#if 0
void freechat_handler_user_rsp(uint8_t *data, uint16_t data_len,
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
#endif
