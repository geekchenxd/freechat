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


int main(int argc, char *argv[])
{
	struct fttp_addr src;
	uint16_t pdu_len = 0;
	uint8_t pdu[MAX_PDU];
	char *name = "eth0";
	pthread_t thid = 0;
	uint8_t opt = 0;

	/*
	 * this demo test user communicate and text trans
	 * argv[1] is the eth name, and the argv[2] is the
	 * opt, the opt == 0 to send 'hello world'
	 */
	if (argc >= 2) {
		name = argv[1];
		if (argc > 2)
			opt = atoi(argv[2]);
	}

	apdu_service_init();
	fttp_set_port(htons(0x20E8));
	if (!fttp_init_udp(name)) {
		debug(ERROR, "Init udp failed!\n");
		exit(1);
	}
	user_id_init();

	send_user_req();
#if 1
	pthread_create(&thid, NULL, user_id_consult, (void *)&opt);
#endif

	while (1) {
		pdu_len = fttp_receive_udp(&src, &pdu[0], MAX_PDU, 3);
		if (pdu_len > 0) {
			debug(DEBUG, "Message received, now to handler it! len = %d\n",
					pdu_len);
			npdu_handler(&src, &pdu[0], pdu_len);
		}

		usleep(1000);
	}
	
	return 0;
}
