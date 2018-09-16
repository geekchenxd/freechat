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
#include "fttp_udp.h"
#include "debug.h"

static int fttp_socket = -1;
static int fttp_port = 0;
static struct in_addr fttp_address;
static struct in_addr fttp_broadcast_addr;

void fttp_set_socket(int fd)
{
	fttp_socket = fd;
}

int fttp_get_socket(void)
{
	return fttp_socket;
}

void fttp_set_addr(uint32_t addr)
{
	/* network byte order */
	fttp_address.s_addr = addr;
}

uint32_t fttp_get_addr(void)
{
	return fttp_address.s_addr;
}

void fttp_set_port(uint16_t port)
{
	fttp_port = port;
}

uint16_t fttp_get_port(void)
{
	return fttp_port;
}

void fttp_set_broadcast_addr(uint32_t addr)
{
	/* network byte order */
	fttp_broadcast_addr.s_addr = addr;
}

/* return network byte order */
uint32_t fttp_get_broadcast_addr(void)
{
	return fttp_broadcast_addr.s_addr;
}

void fttp_get_my_address(struct fttp_addr *my_address)
{
	if (!my_address)
		return;
	my_address->addr_len = 6;
	memcpy(&my_address->addr[0], &fttp_address.s_addr, 4);
	memcpy(&my_address->addr[4], &fttp_port, 2);
}

void fttp_get_broadcast_address(struct fttp_addr *addr)
{
	if (!addr)
		return;

	addr->addr_len = 6;
	memcpy(&addr->addr[0], &fttp_broadcast_addr.s_addr, 4);
	memcpy(&addr->addr[4], &fttp_port, 2);
}

int fttp_decode_address(
		struct fttp_addr *addr,
		struct in_addr *address,
		uint16_t *port)
{
	if (!addr)
		return 0;

	memcpy(&address->s_addr, &addr->addr[0], 4);
	memcpy(port, &addr->addr[4], 2);

	return 6;
}

/*
 * receive udp message from fttp socket, message will ignored if
 * more than the bytes number described by max_pud. this function
 * will block for a period time described by timeout. return -1 
 * on error, or return the bytes received.
 */
uint16_t fttp_receive_udp (
		struct fttp_addr *src, uint8_t *pdu,
		uint16_t max_pdu, uint32_t timeout)
{

	int recv_bytes = 0;
    uint16_t pdu_len = 0;       /* return value */
    fd_set fds;
    int max = 0;
    struct timeval select_timeout;
    struct sockaddr_in sin = { 0 };
    socklen_t sin_len = sizeof(sin);
    uint16_t i = 0;

    /* is the socket is opened? */
    if (fttp_socket < 0)
        return 0;

    if (timeout >= 1000) {
        select_timeout.tv_sec = timeout / 1000;
        select_timeout.tv_usec =
            1000 * (timeout - select_timeout.tv_sec * 1000);
    } else {
        select_timeout.tv_sec = 0;
        select_timeout.tv_usec = 1000 * timeout;
    }
    FD_ZERO(&fds);
    FD_SET(fttp_socket, &fds);
    max = fttp_socket;
    if (select(max + 1, &fds, NULL, NULL, &select_timeout) > 0) {
        recv_bytes =
            recvfrom(fttp_socket, (char *) &pdu[0], max_pdu, 0,
            (struct sockaddr *)&sin, &sin_len);
	} else {
        return 0;
	}

    if (recv_bytes < 0)
        return 0;

    if (recv_bytes == 0)
        return 0;

    /* the signature of a fttp packet */
	if (pdu[0] != (uint8_t)FTTP_SIGNATURE)
		return 0;
	recv_bytes -= 1;

	/* if the message is from myself, ignore it */
	if ((sin.sin_addr.s_addr == fttp_address.s_addr) &&
			(sin.sin_port == fttp_port)) {
		debug(INFO, "Recv Frome Me, ifnaored.\n");
		return 0;
	}

	/* copy the addr to high user */
	src->addr_len = 6;
	memcpy(&src->addr[0], &sin.sin_addr.s_addr, 4);
	memcpy(&src->addr[4], &sin.sin_port, 2);

	for (i = 0; i < recv_bytes; i++) {
		pdu[i] = pdu[i + 1];
		debug(INFO, "0x%x ", pdu[i]);
	}
	debug(INFO, "\n");
	if (recv_bytes < max_pdu)
		pdu_len = recv_bytes;

	return pdu_len;
}

/*
 * Send data pointed by pdu to address pointed by dest.
 * return pdu_len on success, -1 on failed and zero if 
 * input pdu_len is zero
 */
int32_t fttp_send_udp(struct fttp_addr *dest, 
		uint8_t *pdu, uint16_t pdu_len) 
{
	struct sockaddr_in fttp_dest;
	int bytes_sent = 0;
	uint8_t buf[MAX_PDU] = {0x0};

	struct in_addr address;
	uint16_t port = 0;

	if (fttp_socket < 0)
		return fttp_socket;

	if (dest->addr_len == 0) { /*broadcast*/
		address.s_addr = fttp_broadcast_addr.s_addr;
		port = fttp_port;
	} else { /*dest->addr_len == 6*/
		fttp_decode_address(dest, &address, &port);
	}

	fttp_dest.sin_family = AF_INET;
	fttp_dest.sin_addr.s_addr = address.s_addr;
	fttp_dest.sin_port = port;
	memset(&(fttp_dest.sin_zero), '\0', 8);

    buf[0] = FTTP_SIGNATURE;
	memcpy(&buf[1], pdu, pdu_len);
	bytes_sent = sendto(fttp_socket, (char *)buf, pdu_len + 1, 0,
			(struct sockaddr *)&fttp_dest, sizeof(struct sockaddr));
	if (bytes_sent > 0) {
		debug(DEBUG, "UDP send data len = %d\n", bytes_sent);
	} else {
		debug(DEBUG, "UDP send data failed!\n");
		perror("");
	}

	return bytes_sent;
}

static int get_local_ifr_ioctl(char *ifname, 
		struct ifreq *ifr, int opt)
{
	int fd;
	int ret;
	strncpy(ifr->ifr_name, ifname, sizeof(ifr->ifr_name));
	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (fd < 0) {
		ret = fd;
	} else {
		ret = ioctl(fd, opt, ifr);
		close(fd);
	}

	return ret;
}

static int get_local_address_ioctl(char *ifname, 
		struct in_addr *addr, int opt)
{
	struct ifreq ifr = { {{0}} };
	struct sockaddr_in *tcpip_addr;
	int ret;

	ret = get_local_ifr_ioctl(ifname, &ifr, opt);
	if (ret >= 0) {
		tcpip_addr = (struct sockaddr_in *)&ifr.ifr_addr;
		memcpy(addr, &tcpip_addr->sin_addr, sizeof(struct in_addr));
	}

	return ret;
}

void fttp_set_interface(
		char *ifname)
{
	struct in_addr local_addr;
	struct in_addr bcast_addr;
	struct in_addr netmask;
	int ret = 0;

	ret = get_local_address_ioctl(ifname, &local_addr, SIOCGIFADDR);
	if (ret < 0)
		local_addr.s_addr = 0;
	fttp_set_addr(local_addr.s_addr);
	debug(INFO, "Interface:%s, IP address:%s\n", ifname, inet_ntoa(local_addr));

	ret = get_local_address_ioctl(ifname, &netmask, SIOCGIFNETMASK);
	if (ret < 0) {
		bcast_addr.s_addr = ~0;
	} else {
		bcast_addr = local_addr;
		bcast_addr.s_addr |= (~netmask.s_addr);
	}
	fttp_set_broadcast_addr(bcast_addr.s_addr);
	debug(INFO, "Bcast:%s\n", inet_ntoa(bcast_addr));
	debug(INFO, "UDP Port:0x%04x [%hu]\n", ntohs(fttp_get_port()),
			ntohs(fttp_get_port()));
}

bool fttp_init_udp(char *ifname)
{
	int status = 0;
	struct sockaddr_in sin;
	int sockopt = 0;
	int sockfd = -1;

	if (ifname)
		fttp_set_interface(ifname);
	else /*default*/
		fttp_set_interface("eth0");

	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sockfd < 0)
		return false;
	fttp_set_socket(sockfd);

	sockopt = 1;
	status = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, 
			sizeof(sockopt));
	if (status < 0) {
		close(sockfd);
		fttp_set_socket(-1);
		return false;
	}

	/* allow send broadcast */
	status = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &sockopt,
			sizeof(sockopt));
	if (status < 0) {
		close(sockfd);
		fttp_set_socket(-1);
		return false;
	}

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = fttp_get_port();
	memset(&(sin.sin_zero), '\0', sizeof(sin.sin_zero));
	status = bind(sockfd, (const struct sockaddr *)&sin, sizeof(struct sockaddr));
	if (status < 0) {
		close(sockfd);
		fttp_set_socket(-1);
		return false;
	}

	return true;
}
