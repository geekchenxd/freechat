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

static struct client gclient;
int send_text_udp(uint8_t *text, uint16_t text_len, uint16_t user_id)
{
	return fttp_trans_text(text, text_len, user_id);
}

void fttp_task(struct client *p)
{
	struct fttp_addr src;
	uint16_t pdu_len = 0;
	uint8_t pdu[MAX_PDU];
	
	while (1) {
		pdu_len = fttp_receive_udp(&src, &pdu[0], MAX_PDU, 3);
		if (pdu_len > 0) {
			debug(DEBUG, "Message received, now to handler it! len = %d\n",
					pdu_len);
			npdu_handler(&src, &pdu[0], pdu_len);
		}

		usleep(1000);
	}
}


bool fttp_init(char *ifname)
{

	apdu_service_init();
	fttp_set_port(htons(0x20E8));
	if (!fttp_init_udp(ifname)) {
		debug(ERROR, "Init udp failed!\n");
		exit(1);
	}
	user_id_init();
	send_user_req();
	fttp_id_gen_wait(1);
	send_user_req();
	return true;
}

