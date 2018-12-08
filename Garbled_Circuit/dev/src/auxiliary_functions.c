/**
  * \file auxiliary_functions.c
  * \brief Implementation of some auxiliary useful functions
*/


#include "randombytes.h"
#include "paillier.h"
#include "auxiliary_functions.h"
#include "time.h"
#include "parameters.h"
#include "hash.h"
#include "string.h"
/**
  * \fn void H(mpz_t out, mpz_t key)
  *  \brief This function computes the hash of an mpz_t

  * \param[out] out the outputted hash stocked as a mpz_t

  * \param[in] key the value to hash
*/
void H(mpz_t out,mpz_t key){
 unsigned char * output=calloc(KEY_SIZE/4,sizeof(unsigned char));
 unsigned char * input=calloc(64,sizeof(unsigned char));
 mpz_export(input,NULL,1,1,0,0,key);
 sha512(output, input,KEY_SIZE/8);
 mpz_import(out,KEY_SIZE/8,-1,1,0,0,output);
 free(output);
 free(input);
}

/**
  * \fn void gen_alea(mpz_t kA, int r)
  * \brief function generating a random mpz_t of given size

  * \param[out] kA the generated alea

  * \param[in] r a seed
*/
void gen_alea(mpz_t kA, int r) {
  //Random generator initialisation
  gmp_randstate_t seed ;
  gmp_randinit_default (seed);
  gmp_randseed_ui(seed,r);
  mpz_urandomb (kA, seed, KEY_SIZE);
  gmp_randclear(seed);
}

/**
  * \fn void gen_input_key(mpz_t ** kA, mpz_t ** kB, mpz_t offset, int r)
  * \brief This function generates keys for Alice and Bob

  * \param[out] kA    mpz_t double array representing Alice's keys
  * \param[out] kB    mpz_t double array representing Bob's keys

  * \param[in] offset mpz_t representing the offset used in freeXOR optimization
  * \param[in] r      int representing a seed
*/
void gen_input_key(mpz_t ** kA, mpz_t ** kB, mpz_t offset, int r) {
  for (int i=0 ; i<PARAM_L+1 ; i++) {
    gen_alea(kA[i][0],r+i); // Key generation for Alice
    gen_alea(kB[i][0],r-i); // Key generation for Bob
    mpz_xor(kA[i][1],kA[i][0],offset);
    mpz_xor(kB[i][1],kB[i][0],offset);
  }
}

/**
  * \fn void random_bytes_pairs(uint8_t* x , uint8_t* y, uint32_t nb_bytes)
  * \brief This function generates two random values of size nb_bytes bytes

  * \param[out] x       bytes array representing the first generated value
  * \param[out] y       bytes array representing the second generated value

  * \param[in] nb_bytes 32 bytes integer representing the size in bytes of the generated values
*/
void random_bytes_pairs(uint8_t* x , uint8_t* y, uint32_t nb_bytes) {
  uint8_t bytes_array[2*nb_bytes];
  random_bytes(bytes_array,2*nb_bytes);
  memcpy(x,bytes_array,nb_bytes);
  memcpy(y,bytes_array+nb_bytes,nb_bytes);
}

/**
  * \fn uint32_t bits_to_bytes(uint32_t nb_bits)
  * \brief This function converts a bits size to a bytes size

  * \param[in] nb_bits  32 bytes integer representing the value to convert
*/
uint32_t bits_to_bytes(uint32_t nb_bits) {
  return (nb_bits + 7) / 8 ;
}

/**
  * \fn void cmp_Bob_gen_inputs(mpz_t ct_gamma, mpz_t rho, mpz_t ct_Alice, mpz_t Bob)
  * \brief This function generate Bob's new input and Alice's new input ciphertext

  * \param[out] ct_gamma  mpz_t representing new Alice's input ciphertext
  * \param[out] rho       mpz_t representing the new Bob's input
  * \param[in] ct_Alice   mpz_t representing Alice's input ciphertext
  * \param[in] Bob        mpz_t representing Bob's input
*/
void cmp_Bob_gen_inputs(mpz_t ct_gamma, mpz_t rho, mpz_t ct_Alice, mpz_t Bob) {

  srand(time(NULL));
  gmp_randstate_t seed ;
  mpz_t p,q,n,temp,n_squared;

  mpz_inits(p,q,n,temp,n_squared, NULL);
  gmp_randinit_default (seed);
  gmp_randseed_ui(seed,time(NULL));

  mpz_set_str(p,PAILLIER_SK_P,16);
  mpz_set_str(q,PAILLIER_SK_Q,16);
  mpz_mul(n,p,q);
  mpz_mul(n_squared,n,n);

  mpz_urandomb(rho,seed,PARAM_L+PARAM_K);
  mpz_ui_pow_ui(ct_gamma,2,PARAM_L);
  mpz_add(ct_gamma,ct_gamma,rho);
  mpz_sub(ct_gamma,ct_gamma,Bob);
  paillier_encrypt(ct_gamma,ct_gamma);
  mpz_mul(ct_gamma,ct_gamma,ct_Alice);
  mpz_mod(ct_gamma,ct_gamma,n_squared);

  gmp_randclear(seed);
  mpz_clears(p,q,n,temp,n_squared, NULL);
}

/**
  * \fn int mpz_quad_res(mpz_t x,mpz_t q,mpz_t n)
  * \brief This function computes quadratic residuosity

  * \param[out] x mpz_t representing the quadratic residuosity

  * \param[in] q  mpz_t representing the value to compute QR of
  * \param[in] n  mpz_t representing the size of the field on which we compute the QR

  * \return -1 if q is quadratic non-residue mod n,
  * \return  1 if q is quadratic residue mod n,
  * \return 0 if q is congruent to 0 mod n
  */
int mpz_quad_res(mpz_t x,mpz_t q,mpz_t n) { // https://gmplib.org/list-archives/gmp-discuss/2013-April/005303.html David Gillies
  int leg;
  mpz_t tmp,ofac,nr,t,r,c,b;
  unsigned int mod4;
  mp_bitcnt_t twofac=0,m,i,ix;

  mod4=mpz_tstbit(n,0);
  if(!mod4) // must be odd
    return 0;

  mod4+=2*mpz_tstbit(n,1);

  leg=mpz_legendre(q,n);
  if(leg!=1)
    return leg;

  mpz_init_set(tmp,n);

  if(mod4==3) // directly, x = q^(n+1)/4 mod n
    {
    mpz_add_ui(tmp,tmp,1UL);
    mpz_tdiv_q_2exp(tmp,tmp,2);
    mpz_powm(x,q,tmp,n);
    mpz_clear(tmp);
    }
  else // Tonelli-Shanks
    {
    mpz_inits(ofac,t,r,c,b,NULL);

    // split n - 1 into odd number times power of 2 ofac*2^twofac
    mpz_sub_ui(tmp,tmp,1UL);
    twofac=mpz_scan1(tmp,twofac); // largest power of 2 divisor
    if(twofac)
      mpz_tdiv_q_2exp(ofac,tmp,twofac); // shift right

      // look for non-residue
    mpz_init_set_ui(nr,2UL);
    while(mpz_legendre(nr,n)!=-1)
      mpz_add_ui(nr,nr,1UL);

    mpz_powm(c,nr,ofac,n); // c = nr^ofac mod n

    mpz_add_ui(tmp,ofac,1UL);
    mpz_tdiv_q_2exp(tmp,tmp,1);
    mpz_powm(r,q,tmp,n); // r = q^(ofac+1)/2 mod n

    mpz_powm(t,q,ofac,n);
    mpz_mod(t,t,n); // t = q^ofac mod n

    if(mpz_cmp_ui(t,1UL)!=0) // if t = 1 mod n we're done
      {
      m=twofac;
      do
              {
              i=2;
              ix=1;
              while(ix<m)
                  {
                  // find lowest 0 < ix < m | t^2^ix = 1 mod n
                  mpz_powm_ui(tmp,t,i,n); // repeatedly square t
                  if(mpz_cmp_ui(tmp,1UL)==0)
                      break;
                  i<<=1; // i = 2, 4, 8, ...
                  ix++; // ix is log2 i
                  }
              mpz_powm_ui(b,c,1<<(m-ix-1),n); // b = c^2^(m-ix-1) mod n
              mpz_mul(r,r,b);
              mpz_mod(r,r,n); // r = r*b mod n
              mpz_mul(c,b,b);
              mpz_mod(c,c,n); // c = b^2 mod n
              mpz_mul(t,t,c);
              mpz_mod(t,t,n); // t = t b^2 mod n
              m=ix;
              }while(mpz_cmp_ui(t,1UL)!=0); // while t mod n != 1
          }
      mpz_set(x,r);
      mpz_clears(tmp,ofac,nr,t,r,c,b,NULL);
      }

  return 1;
}
