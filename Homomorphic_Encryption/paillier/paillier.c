/**
  * \file paillier.c
  * \brief implementation of Paillier's cryptosystem
*/

#include "paillier.h"

/**
  * \fn void paillier_encrypt(mpz_t c, mpz_t m)
  * \brief This function encrypts a message with a paillier public key

  * \param[out] c mpz_t representing the encrypted value

  * \param[in] m mpz_t representing the message to encrypt
*/
void paillier_encrypt(mpz_t c, mpz_t m) {

  mpz_t n,n_squared,r;
  mpz_inits(n,n_squared,r,NULL);

  mpz_set_str(n,PAILLIER_PK_N,16);
  srand(time(NULL));
  gmp_randstate_t seed ;
  gmp_randinit_default (seed);
  gmp_randseed_ui(seed,time(NULL));

  mpz_mul(n_squared,n,n);

  mpz_urandomm(r,seed,n);
  mpz_mul(c,m,n);
  mpz_add_ui(c,c,1);
  mpz_mod(c,c,n_squared);
  mpz_powm(r,r,n,n_squared);

  mpz_mul(c,c,r);
  mpz_mod(c,c,n_squared);

  mpz_clears(n,n_squared,r,NULL);
  gmp_randclear(seed);
}

/**
  * \fn void paillier_encrypt_ui(mpz_t c, unsigned int m)
  * \brief This function encrypts a message with a paillier public key

  * \param[out] c mpz_t representing the encrypted value

  * \param[in] m mpz_t representing the message to encrypt
*/
void paillier_encrypt_ui(mpz_t c, unsigned int m) {

  mpz_t n,n_squared,r;
  mpz_inits(n,n_squared,r,NULL);

  mpz_set_str(n,PAILLIER_PK_N,16);
  srand(time(NULL));
  gmp_randstate_t seed ;
  gmp_randinit_default (seed);
  gmp_randseed_ui(seed,time(NULL));

  mpz_mul(n_squared,n,n);

  mpz_urandomm(r,seed,n);
  mpz_mul_ui(c,n,m);
  mpz_add_ui(c,c,1);
  mpz_mod(c,c,n_squared);
  mpz_powm(r,r,n,n_squared);

  mpz_mul(c,c,r);
  mpz_mod(c,c,n_squared);

  mpz_clears(n,n_squared,r,NULL);
  gmp_randclear(seed);
}

/**
  * \fn void paillier_decrypt(mpz_t m, mpz_t c)
  * \brief function decrypting a ciphertext with a Paillir key

  * \param[out] m mpz_t representing the decrypted value

  * \param[in] c mpz_t representing the ciphertext to decrypt
*/
void paillier_decrypt(mpz_t m, mpz_t c) {

  mpz_t p,q,n,phi,temp,r,n_squared;
  mpz_inits(p,q,n,phi,temp,r,n_squared, NULL);

  mpz_set_str(p,PAILLIER_SK_P,16);
  mpz_set_str(q,PAILLIER_SK_Q,16);
  mpz_set_str(n,PAILLIER_PK_N,16);;

  mpz_mul(n_squared,n,n);
  mpz_sub_ui(phi,p,1);
  mpz_sub_ui(temp,q,1);
  mpz_mul(phi,phi,temp);

  mpz_invert(r,n,phi);
  mpz_powm(r,c,r,n);

  mpz_invert(r,r,n_squared);
  mpz_powm(r,r,n,n_squared);
  mpz_mul(r,r,c);
  mpz_mod(r,r,n_squared);

  mpz_sub_ui(r,r,1);
  mpz_cdiv_q(m,r,n);

  mpz_clears(p,q,n,phi,temp,r,n_squared, NULL);
}
