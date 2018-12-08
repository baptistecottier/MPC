/**
  * \file key_generation.h
  * \brief functions for generation of DGK keys
*/

#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdlib.h>
#include <stdio.h>
#include "gmp.h"
#include "time.h"
#include "math.h"
#include "parameters.h"

/**
  * \typedef dgk_pk
  * \brief Structure of a DGK public key
*/
typedef struct dgk_pk{
  mpz_t g; /**< The first generator*/
  mpz_t n; /**< The RSA modulo*/
  mpz_t h; /**< The second generator*/
  unsigned int u; /**< An L_SIZE-bits prime */
} dgk_pk;

/**
  * \typedef dgk_sk
  * \brief Structure of a DGK secret key
*/
typedef struct dgk_sk {
  mpz_t p; /**< The first prime factor of n*/
  mpz_t q; /**< The second prime factor of n*/
  mpz_t v_p; /**< T_SIZE-bits prime dividing p-1*/
  mpz_t v_q;/**< T_SIZE-bits prime dividing q-1*/
} dgk_sk;

dgk_pk * dgk_pk_init();
void dgk_pk_clear(dgk_pk * publicKey);
dgk_sk * dgk_sk_init();
void dgk_sk_clear(dgk_sk * secretKey);
void mpz_generator_n(mpz_t gen, mpz_t n, mpz_t * factors);
void mpz_CRT(mpz_t x, mpz_t * cong, mpz_t * moduli, int nb_eq);
void mpz_generator_p_q(mpz_t gen, mpz_t p, mpz_t * p_fact, mpz_t q, mpz_t * q_fact);
void dgk_key_generation(dgk_pk * publicKey,dgk_sk * secretKey);

#endif
