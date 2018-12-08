#include <stdio.h>
#include <stdlib.h>

#include "gmp.h" 


void main(int argc, char * argv[]) {
	mpz_t n1, n2;
	mpz_inits(n1,n2,NULL);
	int sum=0;

	mpz_set_str(n1,argv[1],2);
	mpz_set_str(n2,argv[2],2);
	for(int j=0 ; j<4 ; j++) sum+=mpz_tstbit(n1,j)^mpz_tstbit(n2,j);
	printf("%d\n", sum);
}
