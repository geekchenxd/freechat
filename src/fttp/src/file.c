#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "decode.h"
#include "file.h"
#include "debug.h"
#include "npdu.h"
#include "apdu.h"

static char fttp_file_root[FTTP_MAX_FILE_PATH] = "./";

static char *file_full_name_get(char *filename)
{
	char *file_full_name = NULL;

	if (!filename)
		goto out;
	file_full_name = malloc(FTTP_MAX_FILE_PATH + FTTP_MAX_FILE_NAME);
	if (!file_full_name)
		goto out;

	memcpy(file_full_name, fttp_file_root, strlen(fttp_file_root));
	if (file_full_name[strlen(fttp_file_root) - 1] != '/') {
		file_full_name[strlen(fttp_file_root)] = '/';
		file_full_name[strlen(fttp_file_root) + 1] = '\0';
	}
	memcpy(&file_full_name[strlen(file_full_name)], filename, strlen(filename));

out:
	return file_full_name;
}

void set_file_root_dir(char *root)
{
	if (!root)
		return;

	if (strlen(root) >= FTTP_MAX_FILE_PATH) {
		debug(ERROR, "the file root path is out of range\n");
		return;
	}

	memcpy(fttp_file_root, root, strlen(root));
	if (fttp_file_root[strlen(root) - 1] != '/') {
		fttp_file_root[strlen(root)] = '/';
		fttp_file_root[strlen(root) + 1] = '\0';
	} else {
		fttp_file_root[strlen(root)] = '\0';
	}
}
	
int file_read_access(char *filename)
{
	int status = 1;
	char *file_full_name = NULL;

	if (!filename) {
		status = 0;
		goto out;
	}
	
	file_full_name = file_full_name_get(filename);
	if (!file_full_name)
		goto out;

	if (access(file_full_name, F_OK) == -1) {
		status--;
		goto out;
	}
	if (access(file_full_name, R_OK) == -1)
		status--;

out:
	if (file_full_name)
		free(file_full_name);
	return status;
}

/*
 * get file size, this function can quickly
 * get the file size, it gets the size of 
 * file by reading the stat of the file intead
 * of opening file and offseting the file pos.
 */
int get_file_size(char *file)
{
	int size = 0;

	struct stat statbuf;
	stat(file, &statbuf);
	size = statbuf.st_size;

	return size;
}

/*
 * if the size of the file is less than
 * the default value of max file size.
 */
bool file_size_valid(char *filename)
{
	bool status = false;
	char *file_full_name = NULL;
	int len = 0;

	if (!filename)
		goto out;
	
	file_full_name = file_full_name_get(filename);
	if (!file_full_name)
		goto out;
	len = get_file_size(file_full_name);
	if (len > 0 && len < MAX_FILE_SIZE)
		status = true;

out:
	if (file_full_name)
		free(file_full_name);
	return status;
}

void handler_trans_file(uint8_t *data, uint16_t data_len,
		struct fttp_addr *src, uint8_t session_id)
{

}


int fttp_filename_decode(uint8_t *data, char *name)
{
	if (!data || !name)
		return 0;
	int len = 0;

	len = fttp_decode_string(data, name);
	if (len > 0)
		name[len] = '\0';

	return len;
}

int fttp_rsp_file_req(struct fttp_addr *dest,
		bool agree, int access, bool valid, uint8_t session_id)
{
	uint8_t npdu[MAX_PDU - 1];
	int len = 0;
	int data_len = 0;
	
	len = encode_npdu(npdu);
	data_len += len;
	len = encode_apdu_common(&npdu[data_len], 
			FTTP_PDU_RSP_COMPLEX, FTTP_SERVICE_FILE_RD_RSP);
	data_len += len;
	npdu[data_len++] = session_id;

	npdu[data_len++] = (uint8_t)agree;
	npdu[data_len++] = (uint8_t)access;
	npdu[data_len++] = (uint8_t)valid;

	/*send a ack*/
	return fttp_send_udp(dest, npdu, data_len);
}

void handler_file_read_req(uint8_t *data, uint16_t data_len,
		struct fttp_addr *src, uint8_t session_id)
{
	char name[FTTP_MAX_FILE_NAME] = {0x0};
	uint16_t decode_len = 0;
	int len = 0;
	uint16_t user_id = 0;
	bool agree = true;
	bool valid = true;
	int access = 1;

	/*
	 * the data[0] is the user's id
	 */
	len = fttp_decode_id(&data[0], &user_id);
	decode_len += len;
	debug(DEBUG, "Recv File read Request From IP:%d.%d.%d.%d\n", 
			src->addr[0], src->addr[1], src->addr[2], src->addr[3]);
	debug(DEBUG, "User ID is %d\n", user_id);

	/*
	 * here we should find the user from current users table to make
	 * sure that the user has connected.
	 */

	len = fttp_filename_decode(&data[decode_len], &name[0]);
	decode_len += len;
	if (decode_len != data_len) {
		debug(ERROR, "Decode fttp file name failed!\n");
		perror("");
		return;
	}
	/*handle the text*/
	debug(INFO, "the request file name is [%s]\n", name); 

	if (file_read_access(name) <= 0) {
		agree = false;
		access = false;
		valid = false;
		goto out;
	}

	if (!file_size_valid(name)) {
		agree = false;
		valid = false;
	}

	/*here add file worker*/

out:
	fttp_rsp_file_req(src, agree, access, valid, session_id);
}

void handler_file_read_rsp(uint8_t *data, uint16_t data_len,
		struct fttp_addr *src, uint8_t session_id)
{
}

