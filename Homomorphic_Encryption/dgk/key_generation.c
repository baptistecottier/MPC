/**
	* \file key_generation.c
	* \brief Implementation of key_generation.h
*/

#include "key_generation.h"

/**
	* \fn dgk_pk * dgk_pk_init()
	* \brief This function initializes a DGK public key

	* \return publicKey dgk_pk stocking public key values
*/
dgk_pk * dgk_pk_init() {
	dgk_pk * publicKey = (dgk_pk *) malloc(sizeof(dgk_pk));
	mpz_inits(publicKey->n,publicKey->g,publicKey->h,NULL);
	return publicKey;
}

/**
	* \fn void dgk_pk_clear(dgk_pk * publicKey)
	* \brief This function clears a DGK public key

	* \param[in] publicKey dgk_pk representing the public key to clear
*/
void dgk_pk_clear(dgk_pk * publicKey) {
	mpz_clears(publicKey->n,publicKey->g,publicKey->h,NULL);
	free(publicKey);
}

/**
	* \fn dgk_sk * dgk_sk_init()
	* \brief This function initializes a DGK secret key

	* \return secretKey dgk_sk stocking secret key values
*/
dgk_sk * dgk_sk_init() {
	dgk_sk * secretKey = (dgk_sk *) malloc(sizeof(dgk_sk));
	mpz_inits(secretKey->p,secretKey->q,secretKey->v_p,secretKey->v_q,NULL);
	return secretKey;
}

/**
	* \fn void dgk_sk_clear(dgk_sk * secretKey)
	* \brief This function clears a DGK secret key

	* \param[in] secretKey dgk_sk representing the secret key to clear
*/
void dgk_sk_clear(dgk_sk * secretKey) {
	mpz_clears(secretKey->p,secretKey->q,secretKey->v_p,secretKey->v_q,NULL);
	free(secretKey);
}
/**
	* \fn void mpz_generator_n(mpz_t gen, mpz_t n, mpz_t * factors)
	* \brief This function finds a generator of Z_n^*

	* \param[out] gen 		mpz_t representing the generated generator

	* \param[in] n				mpz_t representing the characteristic of the considered field
	* \param[in] factors mpz_t array representing prime factors of n

*/
void mpz_generator_n(mpz_t gen, mpz_t n, mpz_t * factors) {
	gmp_randstate_t seed ;
	gmp_randinit_default (seed);
	gmp_randseed_ui(seed,rand());

	int t=0;

	mpz_t alea,b,pow;
	mpz_inits(alea,b,pow,NULL);

	while(t==0 || mpz_cmp_ui(alea,1)==0) {
		mpz_urandomm(alea,seed,n);

		for (int i=0 ; i<4; i++) {
			mpz_divexact(pow,n,factors[i]);
			mpz_powm(b,alea,pow,n);
			if (mpz_cmp_ui(b,1)==0) break;
			t=1 ;
		}
	}
	mpz_set(gen,alea);

	mpz_clears(alea,b,pow,NULL);
	gmp_randclear(seed);
}

/**
	* \fn void mpz_CRT(mpz_t x, mpz_t * cong, mpz_t * moduli, int nb_eq)
	* \brief This function computes the Chinese Remainder Theorem

	* \param[out] x 			mpz_t representing the system solution
	* \param[in] cong 		mpz_t array representing the congruences
	* \param[in] moduli 	mpz_t array representing the moduli
	* \param[in] nb_eq 		int representing the equation amount
*/
void mpz_CRT(mpz_t x, mpz_t * cong, mpz_t * moduli, int nb_eq) {

	int mod=1;
	mpz_t N, N_i, d_i;
	mpz_inits(N,N_i,d_i,NULL);

	mpz_set_ui(N,1);
	for (int i=0 ; i<nb_eq ; i++) mpz_mul(N,N,moduli[i]);
	for (int i=0 ; i<nb_eq ; i++) {
		mpz_divexact(N_i,N,moduli[i]);
		mpz_invert(d_i,N_i,moduli[i]);
		mpz_mul(d_i,d_i,N_i);
		mpz_mul(d_i,d_i,cong[i]);
		mpz_add(x,x,d_i);
	}
	mpz_mod(x,x,N);

	mpz_clears(N,N_i,d_i,NULL);

}

/**
	* \fn void mpz_generator_p_q(mpz_t gen, mpz_t p, mpz_t * p_fact, mpz_t q, mpz_t * q_fact)
	* \brief This function selects an element of maximum order in Z_pq^*

	* \param[out] gen 	mpz_t representing the selected element with maximum order

	* \param[in] p 			mpz_t representing the value p
	* \param[in] p_fact	mpz_t array representing the prime factors of p
	* \param[in] q 			mpz_t representing the value q
	* \param[in] q_fact	mpz_t array representing the prime factors of q

*/
void mpz_generator_p_q(mpz_t gen, mpz_t p, mpz_t * p_fact, mpz_t q, mpz_t * q_fact) {
	mpz_t gen_p,gen_q,pm,qm;
	mpz_t * cong=calloc(2,sizeof(mpz_t));
	mpz_t * moduli=calloc(2,sizeof(mpz_t));

	mpz_inits(gen_p,gen_q,pm,qm,NULL);
	for (int k=0; k<2;k++) mpz_inits(cong[k],moduli[k],NULL);

	mpz_sub_ui(pm,p,1);
	mpz_sub_ui(qm,q,1);

	mpz_generator_n(gen_p,pm,p_fact);

	mpz_generator_n(gen_q,qm,q_fact);

	mpz_set(cong[0],gen_p);
	mpz_set(cong[1],gen_q);
	mpz_set(moduli[0],p);
	mpz_set(moduli[1],q);
	mpz_CRT(gen,cong,moduli,2);

	mpz_clears(gen_p,gen_q,pm,qm,NULL);
	for (int k=0; k<2;k++) mpz_clears(cong[k],moduli[k],NULL);
	free(cong);
	free(moduli);
}

/**
	* \fn void dgk_key_generation(dgk_pk * publicKey,dgk_sk * secretKey)
	* \brief This function generates a DGK secret key and public key

	* \param[out] publicKey 	dgk_pk stocking public key values
	* \param[out] secretKey 	dgk_sk stocking secret key values
*/
void dgk_key_generation(dgk_pk * publicKey,dgk_sk * secretKey) {

	unsigned int RAND_SIZE=K_SIZE/2-T_SIZE-L_SIZE;
	mpz_t temp,p_r,pp_r,q_r,qp_r,LCM;
	mpz_inits(temp,p_r,pp_r,q_r,qp_r,LCM,NULL);

	mpz_t * p_fact=calloc(4,sizeof(mpz_t));
	mpz_t * q_fact=calloc(4,sizeof(mpz_t));
	for (int i=0;i<4;i++) mpz_inits(p_fact[i],q_fact[i],NULL);

	gmp_randstate_t seed ;
	gmp_randinit_default (seed);
	gmp_randseed_ui(seed,rand());

	publicKey->u=1<<(L_SIZE+2);

	//p_r generation and p computation
	do {
		mpz_urandomb(pp_r,seed,RAND_SIZE);
	} while (mpz_probab_prime_p(pp_r,PRIME_TEST_ITERATIONS)==0);
	mpz_mul_ui(p_r,pp_r,2);

	do {
		do {
			mpz_urandomb(secretKey->v_p,seed,T_SIZE);
		} while (mpz_probab_prime_p(secretKey->v_p,PRIME_TEST_ITERATIONS)==0) ;
		mpz_mul_ui(secretKey->p,p_r,publicKey->u);
		mpz_mul(secretKey->p,secretKey->p,secretKey->v_p);
		mpz_add_ui(secretKey->p,secretKey->p,1);

	} while (mpz_probab_prime_p(secretKey->p,PRIME_TEST_ITERATIONS)==0);


	//q_r generation and q computation
	do {
		mpz_urandomb(qp_r,seed,RAND_SIZE);
	} while (mpz_probab_prime_p(qp_r,PRIME_TEST_ITERATIONS)==0);

	mpz_mul_ui(q_r,qp_r,2);

	do {
		do {
			mpz_urandomb(secretKey->v_q,seed,T_SIZE);
		} while (mpz_probab_prime_p(secretKey->v_q,PRIME_TEST_ITERATIONS)==0) ;
		mpz_mul_ui(secretKey->q,q_r,publicKey->u);
		mpz_mul(secretKey->q,secretKey->q,secretKey->v_q);
		mpz_add_ui(secretKey->q,secretKey->q,1);

	} while (mpz_probab_prime_p(secretKey->q,PRIME_TEST_ITERATIONS)==0);


	mpz_mul(LCM,pp_r,qp_r);
	mpz_mul(LCM,LCM,secretKey->v_p);
	mpz_mul(LCM,LCM,secretKey->v_q);
	mpz_mul_ui(LCM,LCM,publicKey->u);
	mpz_mul_ui(LCM,LCM,2);


	mpz_set_ui(p_fact[0],2);
	mpz_set(p_fact[1],pp_r);
	mpz_set(p_fact[2],secretKey->v_p);
	mpz_set_ui(p_fact[3],publicKey->u);

	mpz_set_ui(q_fact[0],2);
	mpz_set(q_fact[1],qp_r);
	mpz_set(q_fact[2],secretKey->v_q);
	mpz_set_ui(q_fact[3],publicKey->u);

	mpz_generator_p_q(publicKey->h,secretKey->p,p_fact,secretKey->q,q_fact);
	mpz_generator_p_q(publicKey->g,secretKey->p,p_fact,secretKey->q,q_fact);


	mpz_mul(publicKey->n,secretKey->p,secretKey->q);

	mpz_mul(temp,pp_r,qp_r);
	mpz_mul_ui(temp,temp,2);

	mpz_powm(publicKey->g,publicKey->g,temp,publicKey->n);

	mpz_mul_ui(temp,temp,publicKey->u);
	mpz_powm(publicKey->h,publicKey->h,temp,publicKey->n);

	mpz_clears(temp,p_r,pp_r,q_r,qp_r,LCM,NULL);
	for (int i=0;i<4;i++) mpz_clears(p_fact[i],q_fact[i],NULL);
	free(p_fact);
	free(q_fact);
	gmp_randclear(seed);
}
