/**
	* \file dgk.c
	* \brief implementation of dgk.h
*/
#include "dgk.h"

//gmp_randstate_t seed ;

/**
	* \fn void dgk_encrypt_ui(mpz_t cipher, unsigned int plain, dgk_pk * publicKey)
	* \brief This function encrypts a plaintext thanks to the public key

	* \param[out] cipher		mpz_t representing encrypted value

	* \param[in] plain 			unsigned int represeting the plaintext to encrypt
	* \param[in] publicKey	dgk_pk stocking the public key values
*/
void dgk_encrypt_ui(mpz_t cipher, unsigned int plain, dgk_pk * publicKey) {

	gmp_randstate_t seed ;

	mpz_t r,temp;
	mpz_inits(r,temp,NULL);

	gmp_randinit_default (seed);
	gmp_randseed_ui(seed,rand());
	mpz_urandomm(r,seed,publicKey->n);
	mpz_powm_ui(cipher,publicKey->g,plain,publicKey->n);
	mpz_powm(temp,publicKey->h,r,publicKey->n);
	mpz_mul(cipher,cipher,temp);
	mpz_mod(cipher,cipher,publicKey->n);

	mpz_clears(r,temp,NULL);
	gmp_randclear(seed);
}

/**
	* \fn void dgk_encrypt_mpz(mpz_t cipher, mpz_t plain, dgk_pk * publicKey)
	* \brief This function encrypts a plaintext thanks to the public key

	* \param[out] cipher		mpz_t representing encrypted value

	* \param[in] plain 			mpz_t represeting the plaintext to encrypt
	* \param[in] publicKey	dgk_pk stocking the public key values
*/
void dgk_encrypt_mpz(mpz_t cipher, mpz_t plain, dgk_pk * publicKey) {
	mpz_t r,temp;
	mpz_inits(r,temp,NULL);
	gmp_randstate_t seed ;

	gmp_randinit_default (seed);
	gmp_randseed_ui(seed,rand());
	mpz_urandomm(r,seed,publicKey->n);
	mpz_powm(cipher,publicKey->g,plain,publicKey->n);
	mpz_powm(temp,publicKey->h,r,publicKey->n);
	mpz_mul(cipher,cipher,temp);
	mpz_mod(cipher,cipher,publicKey->n);

	mpz_clears(r,temp,NULL);
	gmp_randclear(seed);
}

/**
	* \fn void dgk_precom_decrypt(mpz_t * decrypt_table, dgk_pk * publicKey, dgk_sk * secretKey)
	* \brief This function precomputes the auxiliary table used in decryption

	* \param[out] decrypt_table mpz_t array representing all possibles values

	* \param[in] publicKey			dgk_pk stocking the public key values
	* \param[in] secretKey			dgk_pk stocking the secret key values
*/
void dgk_precom_decrypt(mpz_t * decrypt_table, dgk_pk * publicKey, dgk_sk * secretKey) {
	mpz_t temp;
	mpz_init(temp);

	mpz_powm(temp,publicKey->g,secretKey->v_p,secretKey->p);
	for (int i=0;i<publicKey->u;i++) mpz_powm_ui(decrypt_table[i],temp,i,secretKey->p);

	mpz_clear(temp);
}

/**
	* \fn void dgk_decrypt(mpz_t plain, mpz_t cipher, mpz_t * decrypt_table, dgk_pk * publicKey, dgk_sk * secretKey)
	* \brief This function decrypts a ciphertext thanks to both public and secret keys

	* \param[out] plain 				mpz_t representing the decrypted value

	* \param[in] cipher 				mpz_t representing the encrypted value
	* \param[in] decrypt_table 	mpz_t array representing all possibles values
	* \param[in] publicKey			dgk_pk stocking the public key values
	* \param[in] secretKey			dgk_pk stocking the secret key values
*/
void dgk_decrypt(mpz_t plain, mpz_t cipher, mpz_t * decrypt_table, dgk_pk * publicKey, dgk_sk * secretKey) {
	mpz_t temp1,temp2,v;
	mpz_inits(temp1,temp2,v,NULL);

	mpz_mul(v,secretKey->v_p,secretKey->v_q);
	mpz_mod(v,v,publicKey->n);
	mpz_powm(temp1,publicKey->g,v,publicKey->n);
	mpz_powm(temp2,cipher,v,publicKey->n);

	for (int i=0;i<publicKey->u;i++) {
		mpz_powm_ui(plain,temp1,i,publicKey->n);
		if (mpz_cmp(temp2,plain)==0){
			mpz_set_ui(plain,i);
			break;
		}
	}

	mpz_clears(temp1,temp2,v,NULL);
}

/**
	* \fn int dgk_is_0_encryption(mpz_t cipher, dgk_sk * secretKey)
	* \brief This function tests if a ciphertext is an encryption of 0

	* \param[in] cipher 		mpz_t representing the encrypted value to test
	* \param[in] secretKey	dgk_sk stocking the secretKey values

	* \return 1 if true
	* \return 0 if false
*/
int dgk_is_0_encryption(mpz_t cipher, dgk_sk * secretKey) {
	int r;
	mpz_t temp;
	mpz_init(temp);
	mpz_powm(temp,cipher,secretKey->v_p,secretKey->p);
	if (mpz_cmp_ui(temp,1)==0) r=1;
	else r=0;

	mpz_clear(temp);
	return r;

}
