#include "decode.h"
#include "apdu.h"

int encode_u16(uint8_t *data, uint16_t value)
{
	data[0] = (uint8_t) ((value & 0xff00) >> 8);
	data[1] = (uint8_t) (value & 0x00ff);

	return 2;
}
int decode_u16(uint8_t *data, uint16_t *value)
{
	if (!value || !data)
		return 0;
	*value = (uint16_t)((((uint16_t)data[0]) << 8) & 0xff00);
	*value |= ((uint16_t) (((uint16_t)data[1]) & 0x00ff));

	return 2;
}

int encode_u32(uint8_t *data, uint32_t value)
{
	data[0] = (uint8_t) ((value & 0xff000000) >> 24);
	data[1] = (uint8_t) ((value & 0x00ff0000) >> 16);
	data[2] = (uint8_t) ((value & 0x0000ff00) >> 8);
	data[3] = (uint8_t) (value & 0x000000ff);

	return 4;
}

int decode_u32(uint8_t *data, uint32_t *value)
{
	if (!value || !data)
		return 0;
	*value = ((uint32_t)((((uint32_t)data[0]) << 24) & 0xff000000));
	*value |= ((uint32_t)((((uint32_t)data[1]) << 16) & 0x00ff0000));
	*value |= ((uint32_t)((((uint32_t)data[2]) << 8) & 0x0000ff00));
	*value |= ((uint32_t)(((uint32_t)data[3]) & 0x000000ff));

	return 4;
}

int decode_unsigned(uint8_t *data, uint8_t value_len,
		uint32_t *value)
{
	uint16_t u16_value = 0;

	if (!value)
		return 0;

	switch (value_len) {
	case 1:
		*value = data[0];
		break;
	case 2:
		decode_u16(data, &u16_value);
		*value = u16_value;
		break;
	case 4:
		decode_u32(data, value);
		break;
	default:
		*value = 0;
		break;
	}

	return (int)value_len;
}

int fttp_encode_id(uint8_t *data, uint16_t id)
{
	int encode_len = 0;

	if (!data)
		return 0;

	data[encode_len++] = (uint8_t)FTTP_VALUE_UINT;
	encode_len += encode_u16(&data[encode_len], 2);
	encode_len += encode_u16(&data[encode_len], id);

	return encode_len;
}	

int fttp_decode_id(uint8_t *data, uint16_t *id)
{
	int decode_len = 0;
	int len = 0;
	enum fttp_value_tag tag;
	uint32_t data_len = 0; 

	/*tag*/
	tag = data[decode_len];
	decode_len++;
	if (tag != FTTP_VALUE_UINT)
		return len;

	/*length*/
	len = decode_unsigned(&data[decode_len], 2, &data_len);
	if (len <= 0)
		return len;
	decode_len += len;

	/*value*/
	len = decode_u16(&data[decode_len], id);
	if (len <= 0)
		return len;
	decode_len += len;

	return decode_len;
}

int fttp_encode_string(uint8_t *data, uint8_t *str, uint32_t len)
{
	int encode_len = 0;
	uint16_t i = 0;

	if (!data || !str || !len)
		return 0;
	data[encode_len++] = (uint8_t)FTTP_VALUE_STRING;
	encode_len += encode_u16(&data[encode_len], len);
	for (i = 0; i < len; i++)
		data[encode_len + i] = str[i];
	encode_len += len;

	return encode_len;
}

int fttp_decode_string(uint8_t *data, char *str)
{
	int decode_len = 0;
	int len = 0;
	enum fttp_value_tag tag;
	uint32_t data_len = 0; 
	uint16_t i = 0;

	if (!data || !str)
		return 0;

	/*tag*/
	tag = data[decode_len];
	decode_len++;
	if (tag != FTTP_VALUE_STRING)
		return len;

	/*length*/
	len = decode_unsigned(&data[decode_len], 2, &data_len);
	if (len <= 0)
		return len;
	decode_len += len;

	/*value*/
	for (i = 0; i < data_len; i++)
		str[i] = data[decode_len + i];
	decode_len += data_len;

	return decode_len;
}

int fttp_encode_enum(uint8_t *data, uint8_t value)
{
	if (!data)
		return 0;
	data[0] = (uint8_t)FTTP_VALUE_ENUM;
	data[1] = value;

	return 2;
}

int fttp_decode_enum(uint8_t *data, uint8_t *value)
{
	int decode_len = 0;
	int len = 0;
	enum fttp_value_tag tag;

	if (!data || !value)
		return 0;

	/*tag*/
	tag = data[decode_len];
	decode_len++;
	if (tag != FTTP_VALUE_ENUM)
		return len;

	/*vlaue*/
	*value = data[decode_len];
	decode_len++;

	return decode_len;
}

int fttp_encode_simple_ack(uint8_t *data, uint8_t session_id)
{
	if (!data)
		return 0;

	int len = 0;
	int data_len = 0;

	len = encode_npdu(data);
	data_len += len;
	len += encode_apdu_common(&data[data_len], FTTP_PDU_RSP_SIMPLE,
			FTTP_SERVICE_TRANS_TEXT);
	data_len += len;
	data[data_len++] = session_id;

	return data_len;
}

