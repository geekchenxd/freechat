#include <stdio.h>
#include "client.h"
#include "users.h"
#include "user_id.h"
#include "text.h"
#include "fttp_udp.h"
#include "display.h"
#include "user_list.h"

/*
 * the global is already externed in client.h
 */


/*
 * the fttp apdu handlers for freechat
 */
void freechat_handler_user_req(uint8_t *data, uint16_t data_len,
		struct fttp_addr *src)
{
	struct type_user *me = NULL;

	me = client.user->user;
	if (me->id == 0)
		me->id = fttp_gen_id();
	send_user_rsp(src, me);
}

void freechat_handler_user_rsp(uint8_t *data, uint16_t data_len,
		struct fttp_addr *src)
{
	struct type_user *new;
	struct user_list *user_node = NULL;
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
#if 0
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
#endif

	if (!fttp_user_id_add(src, new->id)) {
		debug(ERROR, "add new user to id cache failed!\n");
		return;
	}

	/*Add the user to user list for freechat
	 * the user is already exist?*/
	user_node = find_user_by_id(client.user, new->id);
	if (user_node)
		return;
	user_node = (struct user_list *)malloc(sizeof(struct user_list));
	if (!user_node) {
		debug(ERROR, "allocate the new user node failed!\n");
		return;
	}
	user_node->online = true;
	user_node->user = new;
	user_node->next = NULL;
	user_list_add(&client.user, user_node);
}


void freechat_handler_trans_text(uint8_t *data, uint16_t data_len,
		struct fttp_addr *src, uint8_t session_id)
{
	uint8_t text[FTTP_MAX_TEXT_SIZE];
	uint16_t decode_len = 0;
	uint16_t encode_len = 0;
	uint16_t user_id = 0;
	int len = 0;
	
#if 0
	debug(DEBUG, "Recv Text From IP:%d.%d.%d.%d\n", src->addr[0],
			src->addr[1], src->addr[2], src->addr[3]);
#endif
	/*
	 * the data[0] is the user's id
	 */
	len = fttp_decode_id(&data[0], &user_id);
	decode_len += len;
#if 0
	debug(DEBUG, "User ID is %d\n", user_id);
#endif

	len = fttp_text_decode(&data[decode_len], data_len - decode_len, &text[0]);
	decode_len += len;

	if (decode_len != data_len) {
		debug(ERROR, "Decode fttp text failed!\n");
		perror("");
		return;
	}
	/*handle the text*/
	/*the text length:len - 3(tag and len)*/
	msg_display(user_id, &text[0], len - 3);

	/*send a simple ack*/
	len = fttp_encode_simple_ack(&text[0], session_id);
	encode_len += len;
	if (encode_len > 0) {
		fttp_send_udp(src, &text[0], encode_len);
	}
}
