#ifndef _FTTP_UDP_H_
#define _FTTP_UDP_H_
#include <stdbool.h>
#include <stdint.h>
#include "fttp_default.h"

void fttp_set_socket(int fd);
int fttp_get_socket(void);
void fttp_set_addr(uint32_t addr);
uint32_t fttp_get_addr(void);
void fttp_set_port(uint16_t port);
uint16_t fttp_get_port(void);
void fttp_set_broadcast_addr(uint32_t addr);
uint32_t fttp_get_broadcast_addr(void);
void fttp_get_my_address(struct fttp_addr *my_address);
void fttp_get_broadcast_address(struct fttp_addr *addr);
uint16_t fttp_receive_udp (
		struct fttp_addr *src, uint8_t *pdu,
		uint16_t max_pdu, uint32_t timeout);
int32_t fttp_send_udp(struct fttp_addr *dest, 
		uint8_t *pdu, uint16_t pdu_len);
bool fttp_init_udp(char *ifname);

#endif
