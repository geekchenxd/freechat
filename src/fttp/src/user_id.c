#include "user_id.h"
#include "debug.h"
#include "apdu.h"
#include <string.h>
#include <unistd.h>

static uint16_t max_id = 0;

static struct user_id_cache id_cache[MAX_ID];

void fttp_id_gen_wait(uint32_t seconds)
{
	if (seconds == 0)
		sleep(apdu_timeout() / 1000);
	else
		sleep(seconds);
}

bool fttp_addr_same(struct fttp_addr *src, struct fttp_addr *dest)
{
	int i = 0;

	if (src->addr_len != dest->addr_len)
		return false;
	for (i = 0; i < MAX_MAC_LEN; i++) {
		if (src->addr[i] != dest->addr[i])
			return false;
	}

	return true;
}

void user_id_init(void)
{
	struct user_id_cache *id = 0;

	id = id_cache;
	while (id <= &id_cache[MAX_ID - 1]) {
		id->flags = 0;
		id++;
	}
	return;
}

bool fttp_addr_get_by_id(struct fttp_addr *dest,
		uint16_t user_id)
{
	struct user_id_cache *idtable;
	bool found = false;

	idtable = id_cache;
	while (idtable <= &id_cache[MAX_ID - 1]) {
		if (((idtable->flags & ADDR_IN_USE) != 0) &&
				(idtable->user_id == user_id)) {
			*dest = idtable->addr;
			found = true;
			break;
		}
		idtable++;
	}

	return found;
}

bool fttp_user_id_add(struct fttp_addr *src, uint16_t id)
{
	struct user_id_cache *idtable;
	bool found = false;

	idtable = id_cache;
	/*if the id is alread exist*/
	while (idtable <= &id_cache[MAX_ID - 1]) {
		if (((idtable->flags & ADDR_IN_USE) != 0) &&
				(idtable->user_id == id)) {
			*src = idtable->addr;
			found = true;
			break;
		}
		idtable++;
	}
	/*if alread exist, update it or return false if the addr
	 * if not same*/
	if (found) {
		if (fttp_addr_same(src, &idtable->addr)) {
			if (id > max_id)
				max_id = id;
			return found;
		} else {
			return false;
		}
	}
	/*if not found, find a new entry and add it*/
	idtable = id_cache;
	while (idtable <= &id_cache[MAX_ID - 1]) {
		if ((idtable->flags /*& ADDR_IN_USE*/) == 0) {
			idtable->user_id = id;
			memcpy(&idtable->addr, src, sizeof(struct fttp_addr));
			idtable->flags |= ADDR_IN_USE;
			found = true;
			break;
		}
		idtable++;
	}
	return found;
}

uint16_t fttp_gen_id(void)
{
	max_id += 1;
	return max_id;
}

bool fttp_user_id_del(uint16_t id)
{
	struct user_id_cache *idtable;

	idtable = id_cache;
	while (idtable <= &id_cache[MAX_ID - 1]) {
		if (((idtable->flags & ADDR_IN_USE) != 0) &&
				(idtable->user_id == id)) {
			idtable->flags = 0;
			return true;
		}
		idtable++;
	}

	return false;
}
