#include "HE_cmp_struct.h"
#include "parameters.h"

cmp_HE_Alice * cmp_HE_Alice_init() {
  cmp_HE_Alice * A = (cmp_HE_Alice *) malloc(sizeof(cmp_HE_Alice));
  mpz_inits(A->input,A->ct_input,A->gamma,A->ct_gamma,A->c,A->ct_tau,A->ct_delta,NULL);
  A->ct_bits_c=calloc(PARAM_L+1,sizeof(mpz_t));
  A->ct_ep=calloc(PARAM_L+1,sizeof(mpz_t));
  for (int i=0 ; i<PARAM_L+1 ; i++) mpz_inits(A->ct_bits_c[i],A->ct_ep[i],NULL);

  return A;
}

cmp_HE_Bob * cmp_HE_Bob_init() {
  cmp_HE_Bob * B = (cmp_HE_Bob *) malloc(sizeof(cmp_HE_Bob));
  mpz_inits(B->input,B->ct_Alice,B->ct_gamma,B->rho,B->ct_d_gamma,B->ct_delta,B->c,B->r,B->ct_tau,B->ct_epsilon,NULL);
  B->ct_bits_c=calloc(PARAM_L+1,sizeof(mpz_t));
  B->ct_e=calloc(PARAM_L+1,sizeof(mpz_t));
  B->ct_ep=calloc(PARAM_L+1,sizeof(mpz_t));
  for (int i=0 ; i<PARAM_L+1 ; i++) mpz_inits(B->ct_bits_c[i],B->ct_e[i],B->ct_ep[i],NULL);

  return B;
}

void cmp_HE_Alice_clear(cmp_HE_Alice * A) {
  mpz_clears(A->input,A->ct_input,A->gamma,A->ct_gamma,A->c,A->ct_tau,A->ct_delta,NULL);
  for (int i=0 ; i<PARAM_L+1 ; i++) mpz_clears(A->ct_bits_c[i],A->ct_ep[i],NULL);
  free(A->ct_bits_c);
  free(A->ct_ep);
  free(A);
}

void cmp_HE_Bob_clear(cmp_HE_Bob * B) {
  mpz_clears(B->input,B->ct_Alice,B->ct_gamma,B->rho,B->ct_d_gamma,B->ct_delta,B->c,B->r,B->ct_tau,B->ct_epsilon,NULL);
  for (int i=0 ; i<PARAM_L+1 ; i++) mpz_clears(B->ct_bits_c[i],B->ct_e[i],B->ct_ep[i],NULL);
  free(B->ct_bits_c);
  free(B->ct_e);
  free(B->ct_ep);
  free(B);
}
