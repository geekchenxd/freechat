#ifndef _FTTP_DEFAULT_H_
#define _FTTP_DEFAULT_H_

#include <stdint.h>

#define FTTP_SIGNATURE 0x68
#define FTTP_PROTOCOL_VERSION 1

#ifndef FTTP_PROTOCOL_REVERSION
#define FTTP_PROTOCOL_REVERSION 0
#endif

#define FTTP_MAX_ADDR (0xFFFF)
#define FTTP_BROADCAST_ADDR (0xFFFF)
#define MAX_MAC_LEN 7

#define FTTPMAXNAMESIZE 40
#define FTTPMAXSIGNATURESIZE 200
#define FTTPMAXBIRTHDAYSIZE	10

#define MAX_APDU 1248
#define MAX_PDU MAX_APDU + 2

#ifndef FTTP_PORT
#define FTTP_PORT 8424 /*0x20E8*/
#endif
#define FTTP_MAX_TEXT_SIZE 200

struct fttp_addr {
	/*addr_len is 0 indicate a broadcast address*/
	uint8_t addr_len;
	/*addr is 4 bytes IP address and 2 bytes port*/
	uint8_t addr[MAX_MAC_LEN];
};

#endif
