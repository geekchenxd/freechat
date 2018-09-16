#ifndef _DECODE_H__
#define _DECODE_H__
#include <stdint.h>
#include "fttp_enum.h"

int encode_u16(uint8_t *data, uint16_t value);
int decode_u16(uint8_t *data, uint16_t *value);
int encode_u32(uint8_t *data, uint32_t value);
int decode_u32(uint8_t *data, uint32_t *value);
int decode_unsigned(uint8_t *data, uint8_t value_len,
		uint32_t *value);
int fttp_encode_id(uint8_t *data, uint16_t id);
int fttp_decode_id(uint8_t *data, uint16_t *id);
int fttp_encode_string(uint8_t *data, uint8_t *str, uint32_t len);
int fttp_decode_string(uint8_t *data, char *str);
int fttp_encode_enum(uint8_t *data, uint8_t value);
int fttp_decode_enum(uint8_t *data, uint8_t *value);
int fttp_encode_simple_ack(uint8_t *data, uint8_t session_id);

#endif
