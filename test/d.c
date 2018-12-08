#include <stdio.h>
#include <stdlib.h>

#include "gmp.h" 

void main(int argc, char * argv[]) {

	mpz_t tmp;
	mpz_init(tmp);
	int n=atoi(argv[1]);
	printf("%d\n",n>>atoi(argv[2]));

	mpz_set_ui(tmp,atoi(argv[1]));
	mpz_fdiv_q_2exp(tmp,tmp,atoi(argv[2]));
	gmp_printf("%Zu \n",tmp);
}
