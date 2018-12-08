/**
  * \file dgk.h
  * \brief functions for DGK encryption, decryption, and O encryption predicate
*/

#ifndef DGK_H
#define DGK_H

#include "key_generation.h"

void dgk_encrypt_ui(mpz_t cipher, unsigned int plain, dgk_pk * publicKey);
void dgk_encrypt_mpz(mpz_t cipher, mpz_t plain, dgk_pk * publicKey);
void dgk_precom_decrypt(mpz_t * decrypt_table, dgk_pk * publicKey, dgk_sk * secretKey);
void dgk_decrypt(mpz_t plain, mpz_t cipher, mpz_t * decrypt_table, dgk_pk * publicKey, dgk_sk * secretKey);
int dgk_is_0_encryption(mpz_t cipher, dgk_sk * secretKey);

#endif
