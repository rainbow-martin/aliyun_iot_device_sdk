/*
 * cryptoutil.h
 *
 * Author
 *     martin.wang     2018/8/7
 *
 */

#ifndef __CRYPTO_UTIL_H__
#define __CRYPTO_UTIL_H__

class CryptoUtil{
public:
	static int HmacEncode(const char* algo, const char* key, unsigned int key_length,
                const char* input, unsigned int input_length,
                unsigned char* &output, unsigned int &output_length);
};

#endif

