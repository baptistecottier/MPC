/**
  * \file auxiliary_functions.h
  * \brief functions useful for other implementations
*/


#ifndef AUXILIARY_FUNCTIONS_H
#define AUXILIARY_FUNCTIONS_H

/*!
  \def max(a,b)
  Computes the maximum of \a a and \a b.
*/
#ifndef max
    #define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#include <stdio.h>
#include <stdlib.h>
#include "openssl/sha.h"
#include "gmp.h"
#include "parameters.h"
#include "paillier.h"
#include <stdint.h>

void H(mpz_t out, mpz_t key);
void gen_alea(mpz_t kA, int r);
void gen_input_key(mpz_t ** kA, mpz_t ** kB, mpz_t offset, int r);
void cmp_Bob_gen_inputs(mpz_t ct_gamma, mpz_t rho, mpz_t Alice, mpz_t Bob);
int mpz_quad_res(mpz_t x,mpz_t q,mpz_t n);
void random_bytes_pairs(uint8_t* x , uint8_t* y, uint32_t nb_bytes);
uint32_t bits_to_bytes(uint32_t nb_bits);

#endif
