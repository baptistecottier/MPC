#ifndef HE_CMP_STRUCT
#define HE_CMP_STRUCT

#include <stdio.h>
#include <stdlib.h>
#include "gmp.h"

typedef struct cmp_HE_Alice {
  mpz_t input ;
  mpz_t ct_input ;
  mpz_t gamma ;
  mpz_t ct_gamma ;
  mpz_t c ;
  mpz_t * ct_bits_c ;
  mpz_t * ct_ep ;
  mpz_t ct_tau ;
  mpz_t ct_delta ;
} cmp_HE_Alice ;

typedef struct cmp_HE_Bob {
  mpz_t input ;
  mpz_t ct_Alice ;
  mpz_t ct_gamma ;
  mpz_t rho ;
  mpz_t ct_d_gamma ;
  mpz_t ct_delta ;
  mpz_t c ; // A SUPPRIMER !!!!!!!
  mpz_t * ct_bits_c ;
  mpz_t r ;
  int s ;
  mpz_t * ct_e ;
  mpz_t * ct_ep ;
  mpz_t ct_tau ;
  mpz_t ct_epsilon ;
} cmp_HE_Bob ;


cmp_HE_Alice * cmp_HE_Alice_init() ;
cmp_HE_Bob * cmp_HE_Bob_init() ;
void cmp_HE_Alice_clear(cmp_HE_Alice * A) ;
void cmp_HE_Bob_clear(cmp_HE_Bob * B) ;

#endif
