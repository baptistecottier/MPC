#include "../src/parameters.h"
#include "../src/comparison.h"
#include "../src/paillier.h"
#include "time.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

inline unsigned long long cpucycles(void) {
  unsigned long long result;
  __asm__ volatile(".byte 15;.byte 49;shlq $32,%%rdx;orq %%rdx,%%rax" : "=a" (result) :: "%rdx");
  return result;
}

int main(){

  //Initialization of the variables
  int result;

  Alice_struct * Alice=cmp_Alice_init();
  OT_sender * Alice_OT = OT_sender_init();

  Bob_struct * Bob=cmp_Bob_init();
  OT_receiver * Bob_OT = OT_receiver_init();

  uint8_t Alice_input[bits_to_bytes(PARAM_L)], Bob_input[bits_to_bytes(PARAM_L)];
  random_bytes_pairs(Alice_input,Bob_input,bits_to_bytes(PARAM_L));

  unsigned long long t_Alice_step1_1 = cpucycles();
  cmp_Alice_step1(Alice_input , Alice , Alice_OT);
  unsigned long long t_Alice_step1_2 = cpucycles();
   //This corresponds to the first network exchange (Alice -> Bob)
  for (int i=0 ; i < PAILLIER_KEY_SIZE/4 ; ++i) Bob->ct_Alice[i]=Alice->ct_Alice[i];
  for (int i=0 ; i<32 ; ++i) Bob_OT->rec_enc_S[i]=Alice_OT->sen_enc_S[i];


  unsigned long long t_Bob_step2_1 = cpucycles();
  cmp_Bob_step2(Bob_input , Bob , Bob_OT);
  unsigned long long t_Bob_step2_2 = cpucycles();
  //This corresponds to the second network exchange (Bob -> Alice)
  for (int i=0 ; i < PAILLIER_KEY_SIZE/4 ; ++i) Alice->ct_gamma[i]=Bob->ct_gamma[i];
  for (int i=0 ; i < 32*(PARAM_L+1) ; ++i) Alice_OT->sen_enc_R[i/32][i%32]=Bob_OT->rec_enc_R[i/32][i%32];


  unsigned long long t_Alice_step3_1 = cpucycles();
  cmp_Alice_step3(Alice , Alice_OT);
  unsigned long long t_Alice_step3_2 = cpucycles();
  //This corresponds to the third network exchange (Alice -> Bob)
  for (int i=0 ; i<2*32 ; ++i) Bob->trans_table[i/32][i%32]=Alice->trans_table[i/32][i%32];
  for (int i=0 ; i<2*(PARAM_L+1) ; ++i) for (int j=0 ; j<32 ; ++j) Bob_OT->rec_keys[i/2][i%2][j]=Alice_OT->sen_keys[i/2][i%2][j];
  for (int i=0 ; i<32*(PARAM_L+1) ; ++i) Bob->Alice_keys[i/32][i%32]=Alice->Alice_keys[i/32][i%32];
  for (int i=0 ; i<2*PARAM_L ; ++i) for (int j=0 ; j<32 ; ++j) Bob->ct_AND[i/2][i%2][j]=Alice->ct_AND[i/2][i%2][j];


  unsigned long long t_Bob_step4_1 = cpucycles();
  result = cmp_Bob_step4(Bob ,  Bob_OT);
  unsigned long long t_Bob_step4_2 = cpucycles();


  printf("\n");
  printf("step1: %lld CPUCYCLES\n", t_Alice_step1_2 - t_Alice_step1_1);
  printf("step2: %lld CPUCYCLES\n", t_Bob_step2_2 - t_Bob_step2_1);
  printf("step3: %lld CPUCYCLES\n", t_Alice_step3_2 - t_Alice_step3_1);
  printf("step4: %lld CPUCYCLES\n", t_Bob_step4_2 - t_Bob_step4_1);
  printf("%d\n",result);
  cmp_Alice_clear(Alice);

}
