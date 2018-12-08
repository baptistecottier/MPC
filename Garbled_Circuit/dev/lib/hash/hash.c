/**
 * \brief A wrapper for OpenSSL SHA512
 */

// #include <stdio.h>
// #include <stdlib.h>
#include <openssl/sha.h>
//#include "gmp.h"

//#define KEY_SIZE 128 /**< Key size used for garbling */

void sha512(unsigned char* output, unsigned char* input, size_t size) {
    SHA512_CTX sha512;
    SHA512_Init(&sha512);
    SHA512_Update(&sha512, input, size);
    SHA512_Final(output, &sha512);
}

