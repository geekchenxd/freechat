#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <net/route.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>
#include "fttp_default.h"
#include "decode.h"
#include "debug.h"
#include "crypt.h"

/*
 * recv the message from the tcp port, if all goes okay,
 * return the real message size, and return 0 or -1 indicate
 * recv error, return 1 indicate recv message not match the
 * fttp procotol.
 */
int fttp_recieve_tcp(
		uint8_t *pdu, uint16_t max_pdu, int socket)
{
	ssize_t recv_bytes = 0;
	int pdu_len = 0;
	uint16_t len_to_recv = 1;

	if (!pdu)
		return 0;
	if (socket <= 0)
		return 0;

	/*recv and check the head of this protocol*/
	do {
		recv_bytes = recv(socket, (void *)pdu, len_to_recv, 0);
		pdu_len = (int)recv_bytes;
		if (recv_bytes <= 0) {
			debug(ERROR, "recv tcp message failed!\n");
			goto out;
		}
		if (pdu[0] != (uint8_t)FTTP_SIGNATURE)
			goto out;
	} while (recv_bytes == 0);

	/*recv the length of this message*/
	do {
		recv_bytes = recv(socket, (void *)pdu, 2, 0);
		pdu_len = (int)recv_bytes;
		if (recv_bytes <= 0) {
			debug(ERROR, "recv tcp message failed!\n");
			goto out;
		}
		if (recv_bytes != 2)
			goto out;
	} while (recv_bytes == 0);

	/*recv the message*/
	decode_u16(pdu, &len_to_recv);
	pdu_len = 0;
	while (len_to_recv) {
		recv_bytes = recv(socket, (void *)&pdu[pdu_len], (int)len_to_recv, 0);
		if (recv_bytes <= 0) {
			debug(ERROR, "recv tcp message failed!\n");
			goto out;
		}
		pdu_len += recv_bytes;
		len_to_recv -= recv_bytes;
	}

out:
	return pdu_len;
}

/*
 * fttp send tcp message, return send size on success, 0 or other
 * on failed.
 */
int fttp_send_tcp(uint8_t *pdu, uint16_t pdu_len, int socket)
{
	int bytes_sent = 0;
	int len = 0;
	uint8_t buf[MAX_PDU] = {0x0};

	if (socket <= 0)
		return socket;

	/*encode the procotol signature*/
    buf[0] = FTTP_SIGNATURE;
	len += 1;
	memcpy(&buf[1], pdu, pdu_len);
	len += pdu_len;
				
	bytes_sent = send(socket, (char *)buf, len, 0);
	if (bytes_sent > 0) {
		debug(DEBUG, "TCP send data len = %d\n", bytes_sent);
	} else {
		debug(DEBUG, "UDP send data failed!\n");
		perror("");
	}

	return bytes_sent;
}
