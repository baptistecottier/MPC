void d(mpz_t d_alpha, mpz_t alpha) {
  mpz_fdiv_q_2exp(d_alpha,alpha,PARAM_L);
}

void step1_Alice(HE_cmp_Alice Alice) {
  paillier_encrypt(Alice->ct_input,Alice->input);
}

void step2_Bob(HE_cmp_Bob Bob) {
  dgk_Bob_gen_inputs(Bob->ct_gamma,Bob->rho,Bob->r,Bob->ct_Alice,Bob->input);
}

void step3_Alice(HE_cmp_Alice Alice) {
  paillier_decrypt(Alice->gamma, Alice->ct_gamma);
  mpz_mod_ui(Alice->c,Alice->gamma,1<<PARAM_L);
  d(d_gamma,Alice->gamma);
  for (int i=0 ; i<PARAM_L;i++) dgk_encrypt_ui(Alice->ct_bits_c[i],mpz_tstbit(Alice->c,i),DGK_pk);
}

void step4_Bob(HE_cmp_Bob Bob) {
  gmp_randinit_default (seed);
  gmp_randseed_ui(seed,rand());

  int sum=0, s=1;
  d(d_rho, Bob->rho);
  mpz_add_ui(tmp1,Bob->d_rho,1);
  mpz_invert(tmp2,Bob->ct_d_gamma,n_squared);
  mpz_mul(tmp1,tmp1,tmp2);
  for (int i=0 ; i<PARAM_L-1;i++) {
    sum=0
    for(int j=i+1 ; j<PARAM_L-1 ; j++) sum+=mpz_tstbit(Bob->c,j)^mpz_tstbit(Bob->r,i);
    tmp=s+mpz_tstbit(Bob->r,i)-mpz_tstbit(Bob->c,i)+3*sum
    dgk_encrypt_ui(Bob->ct_e[i],tmp,DGK_pk);
  }
  for(int j=0 ; j<PARAM_L-1 ; j++) sum+=mpz_tstbit(Bob->c,j)^mpz_tstbit(Bob->r,i);
  tmp=s-1+3*sum;
  dgk_encrypt_ui(Bob->ct_e[PARAM_L],tmp,DGK_pk);

int s_i ;
  for (int i=0 ; i<PARAM_L;i++) {
    s_i = rand%(DGK_pk->u);
    mpz_urandomb(sp_i,seed,2 * T_SIZE);
    mpz_powm(mpz_tmp,DGK_pk->h,sp_i,DGK_pk->n);
    mpz_powm_ui(Bob->ct_ep[i],Bob->ct_e[i],s_i,DGK_pk->n);
    mpz_mul(Bob->ct_ep[i],Bob->ct_ep[i],mpz_tmp);
    mpz_mod(Bob->ct_ep[i],Bob->ct_ep[i],n);
  }
}

void step5_Alice(HE_cmp_Alice Alice) {

  int tau=1 ;
  for (int i=0 ; i<PARAM_L ; i++ ) {
    if (dgk_is_0_encryption(Alice->ct_ep[i], secretKey)) {
      tau=0;
      break;
    }
  }
  paillier_encrypt_ui(Alice->ct_tau,tau);
}

void step6_Bob(HE_cmp_Bob Bob) {
}
