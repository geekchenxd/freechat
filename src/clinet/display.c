#include "display.h"
#include "user_list.h"
#include "fttp.h"
#include "config.h"


void* display_func(void *arg) 
{
	struct client *p = (struct client *)arg;
	char *name = "eth0";

	if (!p)
		return NULL;

	if (p->ifname) {
		if (!fttp_init(p->ifname)) {
			printf("Display init fttp protocol failed\n");
			return NULL;
		}
	} else {
		if (!fttp_init(name)) {
			printf("Display init fttp protocol failed\n");
			return NULL;
		}
	}

	/*
	 * this task no need to return
	 */
	fttp_task(p);

    return NULL;
}

/*
 * display the message received.
 */
void msg_display(uint16_t user_id, uint8_t *text, uint16_t len)
{
	char buf[300] = {0x0};
	int length = 0;
	if (!text || !len)
		return;

	struct user_list *user = NULL;

	user = find_user_by_id(client.user, user_id);
	if (!user)
		return;

	length = strlen(user->user->name);
	memcpy(&buf[0], user->user->name, length);
	memcpy(&buf[length], ">> ", 3);
	length += 3;

	memcpy(&buf[length], (char *)text, len);
	draw_new(client.gui.display, &buf[0]);
}

/*
 * display the message send failed
 */
void freechat_mark_message(char *msg, uint16_t user_id)
{
	int len = 0;
	char buf[300] = {0x0};
	char tmp[MAXNAMESIZE + 20] = {0x0};
	struct user_list *user = NULL;
	int msg_len = 0;

	user = find_user_by_id(client.user, user_id);
	if (!user)
		return;
	
	if (!msg)
		return;
	msg_len = strlen(msg);

	memcpy(&buf[0], "freechat>> ", 11);
	sprintf(&tmp[0], "([to %s]send failed!)", user->user->name);
	strcat(&buf[0], &tmp[0]);
	len = strlen(&buf[0]);

	msg_len = (msg_len < 300) ? msg_len : 300;
	memcpy(&buf[len], msg, msg_len);
	
	draw_new(client.gui.display, &buf[0]);
}
