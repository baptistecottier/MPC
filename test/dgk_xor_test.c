#include <stdio.h>
#include <stdlib.h>
#include "/home/cottier/Documents/git/homomorphic_encryption/dgk/dev/src/dgk.c"
#include "/home/cottier/Documents/git/homomorphic_encryption/dgk/dev/src/key_generation.c"
#include "gmp.h"


void main(int argc, char * argv[]) {

	dgk_pk * publicKey=dgk_pk_init();
	dgk_sk * secretKey=dgk_sk_init();
	dgk_key_generation(publicKey,secretKey);

	mpz_t n1, n2,dgk_1;
	mpz_t * e_n1 = calloc(4,sizeof(mpz_t));

	mpz_inits(n1,n2,dgk_1,NULL);
	int sum=0;

	mpz_set_str(n1,argv[1],2);
	mpz_set_str(n2,argv[2],2);
	for(int j=0 ; j<4 ; j++) sum+=mpz_tstbit(n1,j)^mpz_tstbit(n2,j);
	printf("%d\n", sum);
	sum=0;
	for (int i=0; i<4; i++) {
		mpz_init(e_n1[i]);
		dgk_encrypt_ui(e_n1[i],mpz_tstbit(n1,i),publicKey);
		if (mpz_tstbit(n2,i)) {
			mpz_invert(e_n1[i],e_n1[i],publicKey->n);
			dgk_encrypt_ui(dgk_1,1,publicKey);g
			mpz_mul(e_n1[i],e_n1[i],dgk_1);
		}
		sum+=(1-dgk_is_0_encryption(e_n1[i],secretKey));
	}
	printf("%d\n", sum);
}
