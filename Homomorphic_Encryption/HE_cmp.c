#include "HE_cmp_struct.c"
#include "dgk/dgk.c"
#include "dgk/key_generation.c"
#include "paillier/paillier.c"

gmp_randstate_t seed ;

void d(mpz_t d_alpha, mpz_t alpha) {
  mpz_fdiv_q_2exp(d_alpha,alpha,PARAM_L);
}

void dgk_Bob_gen_inputs(cmp_HE_Bob * Bob) {

  srand(time(NULL));
  mpz_t p,q,n,temp,n_squared;

  mpz_inits(p,q,n,temp,n_squared, NULL);
  gmp_randinit_default(seed);
  gmp_randseed_ui(seed,time(NULL));

  mpz_set_str(p,PAILLIER_SK_P,16);
  mpz_set_str(q,PAILLIER_SK_Q,16);
  mpz_mul(n,p,q);
  mpz_mul(n_squared,n,n);

  mpz_urandomb(Bob->rho,seed,PARAM_L+PARAM_K);
  mpz_ui_pow_ui(Bob->ct_gamma,2,PARAM_L);
  mpz_add(Bob->ct_gamma,Bob->ct_gamma,Bob->rho);
  mpz_sub(Bob->ct_gamma,Bob->ct_gamma,Bob->input);
  paillier_encrypt(Bob->ct_gamma,Bob->ct_gamma);
  mpz_mul(Bob->ct_gamma,Bob->ct_gamma,Bob->ct_Alice);
  mpz_mod(Bob->ct_gamma,Bob->ct_gamma,n_squared);

  mpz_mod_ui(Bob->r,Bob->rho,1<<PARAM_L);

  gmp_randclear(seed);
  mpz_clears(p,q,n,temp,n_squared, NULL);
}

void step1_Alice(cmp_HE_Alice * Alice) {
  paillier_encrypt(Alice->ct_input,Alice->input);
}

void step2_Bob(cmp_HE_Bob * Bob) {
  dgk_Bob_gen_inputs(Bob);
}

void step3_Alice(cmp_HE_Alice * Alice, dgk_pk * DGK_publicKey) {
  mpz_t d_gamma ;
  mpz_init(d_gamma);
  paillier_decrypt(Alice->gamma, Alice->ct_gamma);
  gmp_printf("gamma : %Zu\n",Alice->gamma);
  mpz_mod_ui(Alice->c,Alice->gamma,1<<PARAM_L);
  d(d_gamma,Alice->gamma);
  for (int i=0 ; i<PARAM_L;i++) dgk_encrypt_ui(Alice->ct_bits_c[i],mpz_tstbit(Alice->c,i),DGK_publicKey);

  mpz_clear(d_gamma);
}

void step4_Bob(cmp_HE_Bob * Bob, dgk_pk * DGK_publicKey) {

  mpz_t tmp1, tmp2, d_rho, n, n_squared;
  mpz_inits(tmp1,tmp2,d_rho,n,n_squared,NULL);
  gmp_randinit_default (seed);
  gmp_randseed_ui(seed,rand());

  mpz_set_str(n,PAILLIER_PK_N,16);
  mpz_mul(n_squared,n,n);

  int sum=0,tmp=0;
  Bob->s=1;
  d(d_rho, Bob->rho);
  gmp_printf("rho : %Zu\n", Bob->rho);
  mpz_add_ui(tmp1,d_rho,1);
  mpz_invert(tmp2,Bob->ct_d_gamma,n_squared);
  mpz_mul(Bob->ct_delta,tmp1,tmp2);
  for (int i=0 ; i<PARAM_L;i++) {
    sum=0;
    for(int j=i+1 ; j<PARAM_L ; j++) sum+=mpz_tstbit(Bob->c,j)^mpz_tstbit(Bob->r,i);
    tmp=Bob->s+mpz_tstbit(Bob->r,i)-mpz_tstbit(Bob->c,i)+3*sum;
    dgk_encrypt_ui(Bob->ct_e[i],tmp,DGK_publicKey);
  }
  for(int j=0 ; j<PARAM_L ; j++) sum+=mpz_tstbit(Bob->c,j)^mpz_tstbit(Bob->r,j);
  tmp=Bob->s-1+3*sum;
  dgk_encrypt_ui(Bob->ct_e[PARAM_L],tmp,DGK_publicKey);

  int s_i ;
  mpz_t sp_i ;
  mpz_init(sp_i);
  for (int i=0 ; i<PARAM_L+1;i++) {
    s_i = rand()%(DGK_publicKey->u);
    mpz_urandomb(sp_i,seed,2 * T_SIZE);
    mpz_powm(tmp1,DGK_publicKey->h,sp_i,DGK_publicKey->n);
    mpz_powm_ui(Bob->ct_ep[i],Bob->ct_e[i],s_i,DGK_publicKey->n);
    mpz_mul(Bob->ct_ep[i],Bob->ct_ep[i],tmp1);
    mpz_mod(Bob->ct_ep[i],Bob->ct_ep[i],n);


  }
  gmp_randclear(seed);
  mpz_clears(tmp1,tmp2,d_rho,n,n_squared,sp_i,NULL);

}

void step5_Alice(cmp_HE_Alice * Alice, dgk_sk * DGK_secretKey) {

  int tau=1 ;
  for (int i=0 ; i<=PARAM_L ; i++ ) {
    if (dgk_is_0_encryption(Alice->ct_ep[i], DGK_secretKey)) {
      tau=0;
      break;
    }
  }
  printf("tau : %u\n", tau);
  paillier_encrypt_ui(Alice->ct_tau,tau);
}

void step6_Bob(cmp_HE_Bob * Bob) {
    mpz_t n,n_squared,tmp1,tmp2;
    mpz_inits(n,n_squared,tmp1,tmp2, NULL);

    mpz_set_str(n,PAILLIER_PK_N,16);
    mpz_mul(n_squared,n,n);
  if (!Bob->s) {
    mpz_invert(tmp1,Bob->ct_tau,n);
    paillier_encrypt_ui(tmp2,1);
    mpz_mul(Bob->ct_epsilon,tmp1,tmp2);
    mpz_mod(Bob->ct_epsilon, Bob->ct_epsilon,n);
  }

  mpz_mul(Bob->ct_delta,Bob->ct_delta,Bob->ct_epsilon);
  mpz_mod(Bob->ct_delta,Bob->ct_delta,n_squared);

  mpz_clears(n,n_squared,tmp1,tmp2, NULL);

}

void step7_Alice(cmp_HE_Alice * Alice) {
  mpz_t delta;
  mpz_init(delta);
  gmp_printf("test : %Zu\n",Alice->ct_delta);
  paillier_decrypt(delta,Alice->ct_delta);

  mpz_clear(delta);
}

void main(){
  cmp_HE_Alice * Alice=cmp_HE_Alice_init();
  cmp_HE_Bob * Bob=cmp_HE_Bob_init();
  dgk_pk * DGK_publicKey=dgk_pk_init() ;
  dgk_sk * DGK_secretKey=dgk_sk_init() ;
  dgk_key_generation(DGK_publicKey, DGK_secretKey);
  mpz_set_ui(Bob->input, 3);
  mpz_set_ui(Alice->input, 2);

  step1_Alice(Alice);
  mpz_set(Bob->ct_Alice,Alice->ct_input);

  step2_Bob(Bob);
  mpz_set(Alice->ct_gamma, Bob->ct_gamma);

  step3_Alice(Alice,DGK_publicKey);
  mpz_set(Bob->c,Alice->c);

  step4_Bob(Bob,DGK_publicKey);
  for (int i=0 ; i<=PARAM_L ; i++) mpz_set(Alice->ct_ep[i],Bob->ct_ep[i]);

  step5_Alice(Alice,DGK_secretKey);
  mpz_set(Bob->ct_tau,Alice->ct_tau);

  step6_Bob(Bob);
  mpz_set(Alice->ct_delta,Bob->ct_delta);

  step7_Alice(Alice);

   cmp_HE_Alice_clear(Alice) ;
   cmp_HE_Bob_clear(Bob) ;
   dgk_pk_clear(DGK_publicKey);
   dgk_sk_clear(DGK_secretKey);
}
