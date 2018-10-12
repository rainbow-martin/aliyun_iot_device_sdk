/*
 * cryptoutil.cpp
 *
 * Author
 *     martin.wang     2018/8/7
 *
 */

#include <string.h>
#include <openssl/hmac.h>
#include "cryptoutil.h"

#ifdef POSIX
#define _stricmp strcasecmp 
#endif


int CryptoUtil::HmacEncode(const char* algo, const char* key, unsigned int key_length,
                const char* input, unsigned int input_length,
                unsigned char* &output, unsigned int &output_length)
{
	const EVP_MD * engine = NULL;
	if (_stricmp("sha512", algo) == 0) {
		engine = EVP_sha512();
	}else if(_stricmp("sha256", algo) == 0) {
		engine = EVP_sha256();
	}else if(_stricmp("sha1", algo) == 0) {
		engine = EVP_sha1();
	}else if(_stricmp("md5", algo) == 0) {
		engine = EVP_md5();
	}else if(_stricmp("sha224", algo) == 0) {
		engine = EVP_sha224();
	}else if(_stricmp("sha384", algo) == 0) {
		engine = EVP_sha384();
	}else if(_stricmp("sha", algo) == 0) {
		engine = EVP_sha();
	}else{
		return -1;
	}

	output = (unsigned char*)malloc(EVP_MAX_MD_SIZE);

	HMAC_CTX ctx;
	HMAC_CTX_init(&ctx);
	HMAC_Init_ex(&ctx, key, strlen(key), engine, NULL);
	HMAC_Update(&ctx, (unsigned char*)input, strlen(input));        // input is OK; &input is WRONG !!!

	HMAC_Final(&ctx, output, &output_length);
	HMAC_CTX_cleanup(&ctx);

	return 0;
}