#ifndef __FILE_H_
#define __FILE_H_
#include <stdint.h>
#include <stdbool.h>
#include "fttp_default.h"

#define FTTP_MAX_FILE_NAME 50
#define FTTP_MAX_FILE_PATH 50

void handler_trans_file(uint8_t *data, uint16_t data_len,
		struct fttp_addr *src, uint8_t session_id);
void handler_file_read_req(uint8_t *data, uint16_t data_len,
		struct fttp_addr *src, uint8_t session_id);
void set_file_root_dir(char *root);
int get_file_size(char *file);
int file_read_access(char *filename);
bool file_size_valid(char *filename);
int fttp_filename_decode(uint8_t *data, char *name);


#endif
