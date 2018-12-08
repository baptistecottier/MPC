/**
  * \fn void cmp_Bob_gen_inputs(mpz_t ct_gamma, mpz_t rho, mpz_t ct_Alice, mpz_t Bob)
  * \brief This function generate Bob's new input and Alice's new input ciphertext

  * \param[out] ct_gamma  mpz_t representing new Alice's input ciphertext
  * \param[out] rho       mpz_t representing the new Bob's input
  * \param[in] ct_Alice   mpz_t representing Alice's input ciphertext
  * \param[in] Bob        mpz_t representing Bob's input
*/
void dgk_Bob_gen_inputs(mpz_t ct_gamma, mpz_t rho, mpz_t r, mpz_t ct_Alice, mpz_t Bob) {

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

  mpz_mod_ui(r,rho,1<<PARAM_L);
  gmp_randclear(seed);
  mpz_clears(p,q,n,temp,n_squared, NULL);
}
