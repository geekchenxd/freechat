#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "fttp_udp.h"
#include "apdu.h"
#include "users.h"
#include "fttp_default.h"
#include "npdu.h"
#include "user_id.h"
#include "client.h"
#include "fttp_handle.h"
#include "session.h"
#include "gui.h"

/*
 * the global client is already externed in client.h
 */


static void freechat_set_fttp_service_handle()
{
	apdu_service_init();
	set_trans_text_handle(freechat_handler_trans_text);
	set_user_req_handle(freechat_handler_user_req);
	set_user_rsp_handle(freechat_handler_user_rsp);
}

int send_text_udp(uint8_t *text, uint16_t text_len, 
		uint16_t user_id, uint16_t my_id)
{
	return fttp_trans_text(text, text_len, user_id, my_id);
}

void fttp_task(struct client *p)
{
	struct fttp_addr src;
	uint16_t pdu_len = 0;
	uint8_t pdu[MAX_PDU];
	time_t elapsed_seconds = 0;
	time_t last_seconds = 0;
	time_t current_seconds = 0;
	uint32_t timeout = 0;
	
	last_seconds = time(NULL);
	while (1) {
		current_seconds = time(NULL);
		pdu_len = fttp_receive_udp(&src, &pdu[0], MAX_PDU, 3);
		if (pdu_len > 0) {
			npdu_handler(&src, &pdu[0], pdu_len);
		}
		elapsed_seconds = current_seconds - last_seconds;
		if (elapsed_seconds) {
			fttp_session_process(elapsed_seconds * 1000);
			last_seconds = time(NULL);
		}
		timeout += elapsed_seconds;
		if (timeout >= 10) {
			send_user_req();
			timeout = 0;
		}

#if 0
		/*
		 * if the terminal sise changed, refresh the display
		 * interface
		 */
		if(cots()) {
			reinit_gui();
			p->gui.display = get_display();
			p->gui.input = get_typing();
			p->gui.single_line = get_single_line();
			continue;
		}
#endif
		usleep(1000);
	}
}


bool fttp_init(char *ifname)
{
	/*apdu_service_init();*/
	freechat_set_fttp_service_handle();
	fttp_set_session_handle(freechat_handle_session_failed);
	fttp_set_port(htons(0x20E8));

	if (!fttp_init_udp(ifname)) {
		printf("Init udp failed!\n");
		exit(1);
	}

	user_id_init();
	send_user_req();
	fttp_id_gen_wait(1);
	send_user_req();

	return true;
}

