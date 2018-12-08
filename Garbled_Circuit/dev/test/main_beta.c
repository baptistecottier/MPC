#include "parameters.h"
#include "comparison.h"
#include "paillier.h"
#include "time.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]){

  //Initialization of the variables
  int result;
  mpz_t Alice_input, Bob_input;
  paillier_sk * sk = paillier_sk_init();
  paillier_pk * pk = paillier_pk_init();
  Alice_struct * Alice=cmp_Alice_init();
  Bob_struct * Bob=cmp_Bob_init();
  ted_curve * C = ted_curve_init();
  OT_sender * Alice_OT = OT_sender_init();
  OT_receiver * Bob_OT = OT_receiver_init();

  mpz_inits(Bob_input, Alice_input, NULL);
  mpz_set_str(Alice_input, argv[1],10);
  mpz_set_str(Bob_input, argv[2],10);
  ted_curve_set_str(C,TED_CURVE_D,TED_CURVE_Q,TED_CURVE_P,TED_CURVE_BX,TED_CURVE_BY,16);
  paillier_get_keys(sk,pk);

  paillier_encrypt(Alice->ct_input,Alice_input,pk);
  paillier_encrypt(Bob->ct_Bob,Bob_input,pk);
  OT_sender_setup(Alice_OT->enc_S, Alice_OT->y,Alice_OT->S,Alice_OT->T,C);

   //This corresponds to the first network exchange (Alice -> Bob)
  mpz_set(Bob->ct_Alice,Alice->ct_input);
  strcpy(Bob_OT->enc_S,Alice_OT->enc_S);


  cmp_Bob_gen_inputs(Bob->ct_gamma,Bob->rho,Bob->ct_Alice, Bob->ct_Bob, pk);
  cmp_Alice_garbling(Alice->kA,Alice->kB,Alice->X,Alice->A,Alice->AND_ct);
  OT_receiver_choose(Bob_OT->enc_R,Bob_OT->x,Bob_OT->R,Bob_OT->enc_S,Bob->rho,C);

  //This corresponds to the second network exchange (Bob -> Alice)
  for (int i=0;i<PARAM_L+1;i++) strcpy(Alice_OT->enc_R[i] , Bob_OT->enc_R[i]);
  mpz_set(Alice->ct_gamma, Bob->ct_gamma);


  paillier_decrypt(Alice->gamma, Alice->ct_gamma, sk, pk);
  OT_sender_key_derivation(Alice_OT->keys,Alice->kB,Alice_OT->enc_R,Alice_OT->T,Alice_OT->y,C);
  cmp_Alice_set_keys(Alice->input_keys, Alice->kA, Alice->gamma);
  //This corresponds to the third network exchange (Alice -> Bob)

  mpz_set(Bob->trans_table[0],Alice->X[3*PARAM_L+2][0]);
  mpz_set(Bob->trans_table[1],Alice->X[3*PARAM_L+2][1]);
  for (int i=0;i<PARAM_L;i++) {
     mpz_set(Bob_OT->keys[i][0],Alice_OT->keys[i][0]);
     mpz_set(Bob_OT->keys[i][1],Alice_OT->keys[i][1]);
     mpz_set(Bob->Alice_keys[i],Alice->input_keys[i]);
     mpz_set(Bob->AND_ct[i][0],Alice->AND_ct[i][0]);
     mpz_set(Bob->AND_ct[i][1],Alice->AND_ct[i][1]);
  }
  mpz_set(Bob_OT->keys[PARAM_L][0],Alice_OT->keys[PARAM_L][0]);
  mpz_set(Bob_OT->keys[PARAM_L][1],Alice_OT->keys[PARAM_L][1]);
  mpz_set(Bob->Alice_keys[PARAM_L],Alice->input_keys[PARAM_L]);
  
  OT_receiver_retrieve(Bob->Bob_keys,Bob_OT->keys,Bob_OT->x,Bob_OT->enc_S,C,Bob->rho);

  // Evaluation
  if (PARAM_INEQ%2==0) result=cmp_Bob_eval(Bob->Alice_keys, Bob->Bob_keys, Bob->AND_ct, Bob->trans_table);
  if (PARAM_INEQ%2==1) result=1-cmp_Bob_eval(Bob->Alice_keys, Bob->Bob_keys, Bob->AND_ct, Bob->trans_table);
 
  printf("%d\n",result);

  mpz_clears(Alice_input, Bob_input, NULL);
  cmp_Alice_clear(Alice);
}

