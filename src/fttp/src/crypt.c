#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <string.h>   
#include <stdint.h>
#include "crypt.h"


void fttp_gen_key(char *passwd, int passwd_len,
		unsigned char *key)
{
	PKCS5_PBKDF2_HMAC_SHA1(passwd, passwd_len,
			(const unsigned char *)KEY_SALT, 
			strlen(KEY_SALT), ITERATIONS, KEY_SIZE, 
			key);
}

static int encrypt(unsigned char *in, int in_len,
		const unsigned char *key, const unsigned char *iv,
		unsigned char *out, unsigned char *tag)
{
	unsigned long err_code;
	EVP_CIPHER_CTX *ctx = NULL;
	int len = 0, out_len = 0;

	/* Create and initialise the context */
	if(!(ctx = EVP_CIPHER_CTX_new())) 
		goto err_out;

	/* Initialise the encryption operation. */
	if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL))
		goto err_out;

	/* Set IV length if default 12 bytes (96 bits) is not appropriate */
	if(1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IV_SIZE, NULL))
		goto err_out;

	/* Initialise key and IV */
	if(1 != EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv))
		goto err_out;

	if(in) {
		if(1 != EVP_EncryptUpdate(ctx, out, &len, 
					(const unsigned char *)in, in_len))
			goto err_out;

		out_len = len;
	}

	if(1 != EVP_EncryptFinal_ex(ctx, out + out_len, &len)) 
		goto err_out;

	out_len += len;

	/* Get the tag */
	if(1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag))
		goto err_out;

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	return out_len;

err_out:
	err_code = ERR_get_error();
	char *err = ERR_error_string(err_code, NULL);
	printf("%s\n", err);
	EVP_CIPHER_CTX_cleanup (ctx);
	return 0;
}

static int decrypt(unsigned char *in, int in_len, 
		unsigned char *tag, const unsigned char *key, 
		const unsigned char *iv, unsigned char *out)
{
	EVP_CIPHER_CTX *ctx = NULL;
	int len = 0, out_len = 0, ret;
	unsigned long err_code;

	/* Create and initialise the context */
	if(!(ctx = EVP_CIPHER_CTX_new()))
		goto err_out;

	/* Initialise the decryption operation. */
	if(!EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL))
		goto err_out;

	/* Set IV length. Not necessary if this is 12 bytes (96 bits) */
	if(!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IV_SIZE, NULL))
		goto err_out;

	/* Initialise key and IV */
	if(!EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv))
		goto err_out;


	if(in)
	{
		if(!EVP_DecryptUpdate(ctx, out, &len, in, in_len))
			goto err_out;

		out_len = len;
	}

	/* Set expected tag value. Works in OpenSSL 1.0.1d and later */
	if(!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, tag))
		goto err_out;

	/* Finalise the decryption. A positive return value indicates success,
	 * anything else is a failure - the plaintext is not trustworthy.
	 */
	ret = EVP_DecryptFinal_ex(ctx, out + out_len, &len);

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	if(ret > 0)
	{
		/* Success */
		out_len += len;
		return out_len;
	}
	else
	{
		/* Verify failed */
		return -1;
	}

err_out:
	err_code = ERR_get_error();
	char *err = ERR_error_string(err_code, NULL);
	printf("%s\n", err);
	EVP_CIPHER_CTX_cleanup (ctx);
	return 0;
}

int fttp_encrypt(uint8_t *input, uint32_t input_len, uint8_t *output)
{
	OpenSSL_add_all_algorithms();
	ERR_load_crypto_strings();     


	/* A 256 bit key */
	static unsigned char key[KEY_SIZE] = {0x0};

	/* A 128 bit IV */
	static unsigned char iv[IV_SIZE] = {0x0};

	/* Buffer for the tag */
	unsigned char tag[16];

	int len = 0;

	if (key[0] == 0x0)
		fttp_gen_key(PASSWDDEFAULT, strlen(PASSWDDEFAULT), key);
	RAND_bytes(iv, IV_SIZE);
	memcpy(output, iv, IV_SIZE);

	/* Encrypt the plaintext */
	len = encrypt(input, input_len, key, iv, 
			(unsigned char *)&output[IV_SIZE + sizeof(tag)], tag);

	if (len < 0) {
		ERR_free_strings();
		printf("Encrypted text failed to verify\n");
		return len;
	}
#if 0
	printf("Ciphertext is:\n");
	BIO_dump_fp(stdout, (const char *)&output[32], len);
	printf("Tag is:\n");
	BIO_dump_fp(stdout, (const char *)tag, 16);
#endif

	memcpy(&output[IV_SIZE], tag, sizeof(tag));
	/* Remove error strings */
	ERR_free_strings();

	if (len > 0)
		return len + IV_SIZE + sizeof(tag);
	else 
		return len;
}

int fttp_decrypt(uint8_t *input, uint32_t input_len, uint8_t *output)
{
	OpenSSL_add_all_algorithms();
	ERR_load_crypto_strings();     


	/* A 256 bit key */
	static unsigned char key[KEY_SIZE] = {0x0};

	/* A 128 bit IV */
	static unsigned char iv[IV_SIZE] = {0x0};

	/* Buffer for the tag */
	unsigned char tag[16];

	int len = 0;

	if (key[0] == 0x0)
		fttp_gen_key(PASSWDDEFAULT, strlen(PASSWDDEFAULT), key);


	/*decode the iv*/
	memcpy(&iv[0], input, IV_SIZE);
	input_len -= IV_SIZE;

	/*decode the tag */
	memcpy(&tag[0], &input[IV_SIZE], sizeof(tag));
	input_len -= sizeof(tag);

	/* Decrypt the ciphertext */
	len = decrypt(&input[IV_SIZE + sizeof(tag)], input_len, tag, key, iv, output);
	if(len < 0)
	{
		/* Verify error */
		printf("Decrypted text failed to verify\n");
	}

	/* Remove error strings */
	ERR_free_strings();

	return len;
}

#if 0
int main(int arc, char *argv[])
{
	uint8_t raw[12] = "helloworld2";
	uint8_t encrypt_raw[64] = {0x0};
	uint8_t decrypt[64] = {0x0};
	int len = 0;

	len = fttp_encrypt(raw, 12, encrypt_raw);
	if (len > 0) {
		printf("encrypt successed\n");
	} else {
		printf("encrypt failed!\n");
		return 0;
	}

	len = fttp_decrypt(encrypt_raw, len, decrypt);
	if (len > 0) {
		printf("decrypt successed!\n");
		printf("%s\n", decrypt);
	} else {
		printf("decrypt failed!\n");
		return 0;
	}

	return 0;
}
#endif

