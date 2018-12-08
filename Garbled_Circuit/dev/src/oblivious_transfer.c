/**
  * \file oblivious_transfer.c
  * \brief implementation of an oblivious transfer protocol based on Twisted Edwards Curves
*/

#include "oblivious_transfer.h"

/**
  * \fn int OT_sender_setup(uint8_t * enc_S, mpz_t y, ted_point * S, ted_point * T)
  * \brief first step : setting up the oblivious transfer

  * \param[out] enc_S bytes array representing the encoded point sent to the receiver
  * \param[out] y     mpz_t representing the secret value for the sender
  * \param[out] S     ted_point representing the common point
  * \param[out] T     ted_point representing a point kept secret by the sender
*/
int OT_sender_setup(uint8_t * enc_S , mpz_t y, ted_point * S, ted_point * T) {
  mpz_t TED_C_P ;
  mpz_init(TED_C_P);
  mpz_set_str(TED_C_P,TED_CURVE_P,16);
  ted_point * B = ted_point_init();
  ted_point_set_str(B, TED_CURVE_BX,TED_CURVE_BY,16);

  gmp_randstate_t seed ;
  gmp_randinit_default (seed);
  gmp_randseed_ui(seed,time(NULL)*time(NULL));
  mpz_urandomm (y, seed, TED_C_P);
  ted_point_mult(S,B,y);
  ted_point_mult(T,S,y);
  ted_encode(enc_S,S);

  mpz_clear(TED_C_P);
  ted_point_clear(B);
  gmp_randclear(seed);
  return 0;
}

/**
  * \fn int OT_receiver_choose(uint8_t ** enc_R, mpz_t * x,  ted_point ** R, uint8_t * enc_S , mpz_t receiver)
  * \brief second step : receiver builds his secret values and send them to sender

  * \param[out] enc_R     bytes double array representing the encoded points R sent to sender
  * \param[out] x         mpz_t representing the receiver's secret values
  * \param[out] R         ted_point array representing the points computed by the receiver
  * \param[in] enc_S      bytes array representing the encoded point received by sender
  * \param[in] receiver   mpz_t representing the input to hide
*/
int OT_receiver_choose(uint8_t ** enc_R, mpz_t * x, ted_point ** R, uint8_t * enc_S, mpz_t receiver) {

  mpz_t temp, TED_C_P ;
  mpz_inits(TED_C_P,temp,NULL);
  mpz_set_str(TED_C_P,TED_CURVE_P,16);
  ted_point * B = ted_point_init();
  ted_point * S = ted_point_init();
  ted_point_set_str(B, TED_CURVE_BX,TED_CURVE_BY,16);

  ted_decode(S,enc_S);
  if (ted_curve_in(S)==0) {
    printf("Error. S does not belong to the curve\n");
    return 1;
  }

  gmp_randstate_t seed ;
  gmp_randinit_default(seed);
  gmp_randseed_ui(seed,time(NULL));
  int b;

  ted_point* temp1 = ted_point_init();
  for (int i=0; i<PARAM_L+1;++i) {
    mpz_urandomm(x[i], seed, TED_C_P);
    ted_point_mult(R[i],B,x[i]);
    b = mpz_tstbit(receiver,i);
    if (b==1) {
      ted_point_add(temp1,R[i],S);
      mpz_set(R[i]->x,temp1->x);
      mpz_set(R[i]->y,temp1->y);
    }
    ted_encode(enc_R[i],R[i]);

  }
  mpz_clears(TED_C_P,temp,NULL);
  ted_point_clear(temp1);
  ted_point_clear(B);
  ted_point_clear(S);
  gmp_randclear(seed);
  return 0;
}

/**
  * \fn int OT_sender_key_derivation(mpz_t ** K, mpz_t ** kB, uint8_t ** enc_R, ted_point * T, mpz_t y)
  * \brief third step : sender computes the values allowing receiver to retrieve his keys

  * \param[out] K     mpz_t array representing the keys the sender send to the receiver
  * \param[in] kB     mpz_t double array representing the receiver's keys
  * \param[in] enc_R  bytes double array representing the encoded points received from receiver
  * \param[in] T      ted_point representing the sender's secret point
  * \param[in] y      mpz_t representing the sender's secret value
*/
int OT_sender_key_derivation(mpz_t ** K, mpz_t ** kB, uint8_t ** enc_R, ted_point * T, mpz_t y) {

  ted_point * temp1 = ted_point_init();
  ted_point * temp2 = ted_point_init();
  ted_point * opT = ted_point_init();
  ted_point ** R = calloc(PARAM_L+1,sizeof(ted_point *));
  for (int i=0; i<PARAM_L+1;++i) {
    R[i]=ted_point_init();
    ted_decode(R[i],enc_R[i]);
    if (ted_curve_in(R[i])==0) {
      printf("Error, at least one of the R value does not belong the curve\n");
      return 1;
    }
    ted_point_mult(temp1,R[i],y); //temp1=yR
    ted_point_opp(opT,T);
    ted_point_add(temp2,temp1,opT);

    mpz_add(K[i][0],temp1->y,temp1->x);
    mpz_add(K[i][1],temp2->y,temp2->x);
    H(K[i][0],K[i][0]);
    H(K[i][1],K[i][1]);

    mpz_xor(K[i][0],K[i][0],kB[i][0]);
    mpz_xor(K[i][1],K[i][1],kB[i][1]);
  }

  ted_point_clear(opT);
  ted_point_clear(temp1);
  ted_point_clear(temp2);
  for (int i = 0; i < PARAM_L+1; ++i) ted_point_clear(R[i]);
  free(R);
  return 0;
}

/**
  * \fn int OT_receiver_retrieve(mpz_t * receiver_input_keys, mpz_t ** K, mpz_t* x, uint8_t * enc_S, mpz_t rho)
  * \brief fourth step : receiver retrieves his input keys

  * \param[out] receiver_input_keys mpz_t array representing the computed keys (the output)
  * \param[in] K                    mpz_t double array representing the received values from sender
  * \param[in] x                    mpz_t array representing the receiver's secret values
  * \param[in] enc_S                bytes array representing the encoded point received from sender
  * \param[in] rho                  mpz_t representing the receiver's new input
*/
int OT_receiver_retrieve(mpz_t * receiver_input_keys, mpz_t ** K, mpz_t * x, uint8_t * enc_S, mpz_t rho) {

  mpz_t k_receiver;
  ted_point * temp1 = ted_point_init();
  ted_point * S = ted_point_init();
  int b;

  mpz_init(k_receiver);
  ted_decode(S,enc_S);

  for (int i=0; i<PARAM_L+1;++i) {
    b = mpz_tstbit(rho,i);
    ted_point_mult(temp1,S,x[i]);
    mpz_add(k_receiver,temp1->y,temp1->x);
    H(k_receiver,k_receiver);
    mpz_xor(receiver_input_keys[i],K[i][b],k_receiver);
  }
  mpz_clear(k_receiver);
  ted_point_clear(temp1);
  ted_point_clear(S);

  return 0;
}

/**
  * \fn OT_sender * OT_sender_init()
  * \brief This function initalizes an OT_sender variable

  * \return OTS an initialized OT_sender variable
*/
OT_sender * OT_sender_init() {

  OT_sender * OTS = (OT_sender * ) malloc(sizeof(OT_sender));

  OTS->sen_y=calloc(bits_to_bytes(255),sizeof(uint8_t));
  OTS->sen_S = ted_point_init();
  OTS->sen_enc_S = calloc(64,sizeof(uint8_t));
  OTS->sen_T = ted_point_init();
  OTS->sen_enc_R = calloc(PARAM_L+1 , sizeof(uint8_t *));
  OTS->sen_keys = calloc(PARAM_L+1, sizeof(uint8_t **));
  for (int i=0 ; i<PARAM_L +1 ; ++i) {
    OTS->sen_enc_R[i]=calloc(64,sizeof(uint8_t));
    OTS->sen_keys[i]=calloc(2 , sizeof(uint8_t *));
    for (int j=0 ; j<2 ; j++) OTS->sen_keys[i][j]=calloc(bits_to_bytes(KEY_SIZE),sizeof(uint8_t));
  }

  return OTS;

}

/**
  * \fn void OT_sender_clear(OT_sender * OTS)
  * \brief This function releases an OT_sender structure

  * \param[in] OTS the variable to release
*/
void OT_sender_clear(OT_sender * OTS) {

  for (int i=0 ; i<PARAM_L+1 ; ++i) {
    free(OTS->sen_enc_R[i]);
    free(OTS->sen_keys[i][0]);
    free(OTS->sen_keys[i][1]);
    free(OTS->sen_keys[i]);
  }
  ted_point_clear(OTS->sen_S);
  ted_point_clear(OTS->sen_T);
  free(OTS->sen_keys);
  free(OTS->sen_enc_R);
  free(OTS->sen_y);
  free(OTS->sen_enc_S);
  free(OTS);
}

/**
  * \fn OT_receiver * OT_receiver_init()
  * \brief This Function initalizes an OT_receiver variable

  * \return OTR an initialized OT_receiver variable
*/
OT_receiver * OT_receiver_init() {

  OT_receiver * OTR = (OT_receiver * ) malloc(sizeof(OT_receiver));

  OTR->rec_enc_S = calloc(bits_to_bytes(TED_CURVE_SIZE),sizeof(uint8_t));
  OTR->rec_keys = calloc(PARAM_L+1, sizeof(uint8_t **));
  OTR->rec_R = calloc(PARAM_L +1 , sizeof(ted_point *));
  OTR->rec_enc_R = calloc(PARAM_L+1 , sizeof(uint8_t *));
  OTR->rec_x = calloc(PARAM_L +1 , sizeof(uint8_t *));

  for (int i=0 ; i<PARAM_L +1 ; ++i) {
    OTR->rec_R[i]=ted_point_init();
    OTR->rec_enc_R[i]=calloc(64,sizeof(uint8_t));
    OTR->rec_keys[i]=calloc(2 , sizeof(uint8_t *));
    OTR->rec_x[i]=calloc(64,sizeof(uint8_t));
    for (int j=0 ; j<2 ; j++) OTR->rec_keys[i][j]=calloc(bits_to_bytes(KEY_SIZE),sizeof(uint8_t));
  }

  return OTR;

}

/**
  * \fn void OT_receiver_clear(OT_receiver * OTR)
  * \brief This function releases an OT_receiver variable

  * \param[in] OTR the variable to release
*/
void OT_receiver_clear(OT_receiver * OTR) {

  for (int i=0 ; i<PARAM_L+1 ; ++i ) {
    ted_point_clear(OTR->rec_R[i]);
    free(OTR->rec_enc_R[i]);
    free(OTR->rec_keys[i][0]);
    free(OTR->rec_keys[i][1]);
    free(OTR->rec_keys[i]);
    free(OTR->rec_x[i]);
  }
  free(OTR->rec_R);
  free(OTR->rec_keys);
  free(OTR->rec_enc_R);
  free(OTR->rec_x);
  free(OTR->rec_enc_S);
  free(OTR);
}
