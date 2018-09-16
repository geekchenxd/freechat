/*
 * session menagement
 */
#include <string.h>
#include "session.h"
#include "apdu.h"

static int session_id = 1;
static struct fttp_session session[MAX_FTTP_SESSION];

/*
 * Get the index of session described by id, id is zero
 * indicate the index of first free session.
 * return value:when id isn't zero,return MAX_FTTP_SESSION
 * indicate the index of this session described by id not
 * fount in current session table; when id is zero, return
 * MAX_FTTP_SESSION indicate current session table if not
 * not available, otherwise return the index of first free 
 * session
 */
static uint8_t session_get_index(uint8_t id)
{
	uint32_t i = 0;
	uint8_t idx = MAX_FTTP_SESSION;

	for (i = 0; i < MAX_FTTP_SESSION; i++) {
		if (session[i].session_id == id) {
			idx = (uint8_t)i;
			break;
		}
	}

	return idx;
}

/*
 * get the next free session's session_id
 */
uint8_t fttp_free_session(void)
{
	uint8_t idx = 0;
	uint8_t id = 0;
	bool fount = false;

	if (session_get_index(0) == MAX_FTTP_SESSION)
		goto out;	/*session is not available*/

	while (!fount) {
		idx = session_get_index(session_id);
		if (idx == MAX_FTTP_SESSION) { /*session_id not found so it's available*/
			fount = true;
			idx = session_get_index(0);
			if (idx != MAX_FTTP_SESSION) {
				session[idx].session_id = id = session_id;
				session[idx].waiting = false;
				session[idx].timer = apdu_timeout();
				session_id++;

				if (session_id == 0)
					session_id = 1;
			}
		} else {
			/*session_id found so we try next noe*/
			session_id++;
			if (session_id == 0)
				session_id = 1;
		}
	}

out:
	return id;
}

void fttp_session_set(uint8_t id, struct fttp_addr *dest, 
		uint8_t *pdu, uint16_t pdu_len)
{
	uint8_t idx;
	uint16_t i = 0;

	if (!id)
		return;

	idx = session_get_index(id);
	if (idx == MAX_FTTP_SESSION)
		return;

	session[idx].waiting = true;
	session[idx].retry = 0;
	session[idx].timer = apdu_timeout();
	for (i = 0; i < pdu_len; i++)
		session[idx].pdu[i] = pdu[i];
	session[idx].pdu_len = pdu_len;
	memcpy(&session[idx].dest, dest, 
			sizeof(struct fttp_addr));
}

bool fttp_session_get(uint8_t id, struct fttp_addr *dest, 
		uint8_t *pdu, uint16_t *pdu_len)
{
	uint16_t i = 0;
	uint8_t idx;

	if (!id)
		return false;

	idx = session_get_index(id);
	if (idx == MAX_FTTP_SESSION)
		return false;

	*pdu_len = session[idx].pdu_len;
	for (i = 0; i < *pdu_len; i++)
		pdu[i] = session[idx].pdu[i];

	memcpy(dest, &session[idx].dest, 
			sizeof(struct fttp_addr));

	return true;
}

void fttp_session_process(uint16_t milliseconds)
{
	unsigned int i = 0;

	for (i = 0; i < MAX_FTTP_SESSION; i++) {
		if (!session[i].waiting)
			continue;
		if (session[i].timer > milliseconds)
			session[i].timer -= milliseconds;
		else
			session[i].timer = 0;

		if (session[i].timer == 0) {
			if (session[i].retry < apdu_retry()) {
				session[i].timer = apdu_timeout();
				session[i].retry++;
				/*send pdu*/
			} else {
				/*handle and clear session id*/
				
				session[i].waiting = false;
			}
		}
	}
}

void fttp_session_clear(uint8_t id)
{
	uint8_t idx;
	idx = session_get_index(id);
	if (idx < MAX_FTTP_SESSION) {
		session[idx].waiting = false;
		session[idx].session_id = 0;
	}
}

