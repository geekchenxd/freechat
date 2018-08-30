#ifndef _FTTP_DEFAULT_H_
#define _FTTP_DEFAULT_H_

#include <stdint.h>

#define FTTP_PROTOCOL_VERSION 1

#ifndef FTTP_PROTOCOL_REVERSION
#define FTTP_PROTOCOL_REVERSION 0
#endif

#define FTTP_MAX_ADDR (0xFFFF)
#define FTTP_BROADCAST_ADDR (0xFFFF)
#define MAX_MAC_LEN 7

struct fttp_addr {
	/*addr_len is 0 indicate a broadcast address*/
	uint8_t addr_len;
	/*addr is 4 bytes IP address and 2 bytes port*/
	uint8_t addr[MAX_MAC_LEN];
};

#endif
