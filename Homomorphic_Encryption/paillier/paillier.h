/**
  * \file paillier.h
  * \brief Functions used to implement Paillier's encryption
*/


#ifndef PAILLIER_H
#define PAILLIER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "gmp.h"
#include "parameters.h"

void paillier_encrypt(mpz_t c, mpz_t m);
void paillier_encrypt_ui(mpz_t c, unsigned int m);
void paillier_decrypt(mpz_t m, mpz_t c);

#endif
