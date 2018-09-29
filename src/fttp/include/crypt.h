#ifndef __CRYPT_H_
#define __CRYPT_H_

#define KEY_SIZE 32  /*AES 256 GCM mode.*/
#define KEY_SALT "fttp_v0.1"
#define PASSWDDEFAULT "HelloFttp"
#define IV_SIZE	16
#define ITERATIONS 1000					

void fttp_gen_key(char *passwd, int passwd_len,
		unsigned char *key);
int fttp_encrypt(uint8_t *input, uint32_t input_len, uint8_t *output);
int fttp_decrypt(uint8_t *input, uint32_t input_len, uint8_t *output);

#endif
