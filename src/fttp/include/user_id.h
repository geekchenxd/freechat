#ifndef _USER_ID_H_
#define _USER_DI_H_
#include <stdint.h>
#include <stdbool.h>
#include "fttp_default.h"

struct user_id_cache {
	uint8_t flags;
	uint16_t user_id;
	struct fttp_addr addr;
	uint32_t ttl;
};

#define ADDR_IN_USE 1
#define MAX_ID	128

void user_id_init(void);
bool fttp_addr_get_by_id(struct fttp_addr *dest,
		uint16_t user_id);
bool fttp_user_id_add(struct fttp_addr *src, uint16_t id);
uint16_t fttp_gen_id(void);
bool fttp_user_id_del(uint16_t id);
bool fttp_addr_same(struct fttp_addr *src, struct fttp_addr *dest);
void fttp_id_gen_wait(uint32_t seconds);

#endif
