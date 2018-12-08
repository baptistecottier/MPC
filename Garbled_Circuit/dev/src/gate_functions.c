/**
  * \file gate_functions.c
  * \brief implementation of functions garbling and evaluating AND gates as in [ZRE15]
*/

#include "gate_functions.h"

/**
	* \fn Alice_struct * cmp_Alice_init()
	* \brief This function initalizes an Alice_struct variable

	* \return A an initialized Alice_struct variable
*/
Alice_struct * cmp_Alice_init() {
  Alice_struct * A = (Alice_struct *) malloc(sizeof(Alice_struct)) ;
  A->ct_Alice=calloc(PAILLIER_KEY_SIZE/4,sizeof(uint8_t));
  A->ct_gamma=calloc(PAILLIER_KEY_SIZE/4,sizeof(uint8_t));
  A->Alice_keys=calloc(PARAM_L+1,sizeof(uint8_t *));
  A->trans_table=calloc(2,sizeof(uint8_t *));
  A->ct_AND=calloc(PARAM_L,sizeof(uint8_t **));
  for (int i=0;i<2;i++) A->trans_table[i]=calloc(bits_to_bytes(KEY_SIZE),sizeof(uint8_t));
  for (int i=0 ; i<PARAM_L+1 ; i++) {
   //Variables Initialisation and memory allocation
    if (i<PARAM_L) {
    	A->ct_AND[i]=calloc(2,sizeof(uint8_t *));
    	for (int j=0 ; j<2 ; j++) A->ct_AND[i][j]=calloc(bits_to_bytes(KEY_SIZE),sizeof(uint8_t));
    }
    A->Alice_keys[i]=calloc(bits_to_bytes(KEY_SIZE),sizeof(uint8_t));
  }
  return A;
}

/**
  * \fn void cmp_Alice_clear(Alice_struct * A)
  * \brief This function clears an Alice_struct variable

  * \param[in] A the structure to release
*/
void cmp_Alice_clear(Alice_struct * A) {
  for (int i=0 ; i<PARAM_L+1 ; ++i) {
    if (i<PARAM_L) {
      free(A->Alice_keys[i]);
      free(A->ct_AND[i][0]);
      free(A->ct_AND[i][1]);
      free(A->ct_AND[i]);
    }
  }
  free(A->Alice_keys[PARAM_L]);
  free(A->trans_table[0]);
  free(A->trans_table[1]);
  free(A->ct_Alice);
  free(A->ct_gamma);
  free(A->Alice_keys);
  free(A->trans_table);
  free(A->ct_AND);
  free(A);
}

/**
	* \fn Bob_struct * cmp_Bob_init()
	* \brief This function initializes a Bob_struct variable

	* \return B an initialized Alice_struct variable
*/
Bob_struct * cmp_Bob_init() {
  Bob_struct * B = (Bob_struct *) malloc (sizeof(Bob_struct));
  B->rho=calloc(bits_to_bytes(PARAM_L+PARAM_K),sizeof(uint8_t));
  B->ct_gamma=calloc(PAILLIER_KEY_SIZE/4,sizeof(uint8_t));
  B->ct_Alice=calloc(PAILLIER_KEY_SIZE/4,sizeof(uint8_t));
  B->trans_table=calloc(2,sizeof(uint8_t*));
  B->Alice_keys=calloc(PARAM_L+1,sizeof(uint8_t *));
  B->ct_AND=calloc(PARAM_L,sizeof(uint8_t **));
  for (int i=0;i<2;i++) B->trans_table[i]=calloc(bits_to_bytes(KEY_SIZE),sizeof(uint8_t));
  for (int i=0 ; i<PARAM_L+1 ; i++) {
    B->Alice_keys[i]=calloc(bits_to_bytes(KEY_SIZE),sizeof(uint8_t));
    if (i<PARAM_L) {
			B->ct_AND[i]=calloc(2,sizeof(uint8_t *));
    	for (int j=0 ; j<2 ; j++) B->ct_AND[i][j]=calloc(bits_to_bytes(KEY_SIZE),sizeof(uint8_t));
    }
  }
  return B ;
}

/**
  * \fn void cmp_Bob_clear(Bob_struct * B)
  * \brief This function clears a Bob_struct variable

  * \param[in] B the structure to release
*/
void cmp_Bob_clear(Bob_struct * B) {
	for (int i = 0; i < PARAM_L; ++i)
	{
		free(B->ct_AND[i][0]);
		free(B->ct_AND[i][1]);
		free(B->ct_AND[i]);
    free(B->Alice_keys[i]);
	}
  free(B->Alice_keys[PARAM_L]);
  free(B->trans_table[0]);
  free(B->trans_table[1]);
  free(B->rho);
  free(B->ct_gamma);
  free(B->ct_Alice);
  free(B->trans_table);
  free(B->Alice_keys);
  free(B->ct_AND);
  free(B);
}

/**
  * \fn void cmp_Alice_set_keys(uint8_t ** Alice_input_keys, mpz_t ** kA, mpz_t gamma)
  * \brief This function extracts the keys Alice sent to Bob

  * \param[out] Alice_input_keys  bytes double array representing Alice's input keys

  * \param[in] kA                 mpz_t double array representing Alice's keys
  * \param[in] gamma              mpz_t representing Alice's news input
*/
void cmp_Alice_set_keys(uint8_t ** Alice_input_keys, mpz_t ** kA, mpz_t gamma) {
  for (int i=0;i<PARAM_L+1;i++) mpz_export(Alice_input_keys[i],NULL,-1,1,0,0,kA[i][mpz_tstbit(gamma,i)]);
}

/**
  * \fn void cmp_Alice_garbling(mpz_t ** kA, mpz_t ** kB, mpz_t * trans_table, mpz_t ** ct_AND) {
  * \brief This function garbles a comparison circuit

  * \param[out] kA          mpz_t double array representing the Alice's keys
  * \param[out] kB          mpz_t double array representing the Bob's keys
  * \param[out] trans_table mpz_t array representing the translation table
  * \param[out] ct_AND      mpz_t double array representing the AND gates ciphertexts
*/
void cmp_Alice_garbling(mpz_t ** kA, mpz_t ** kB, mpz_t * trans_table, mpz_t ** ct_AND) {

  mpz_t offset, A;
  mpz_t ** X;
  mpz_inits(offset, A, NULL);
  X=calloc(3,sizeof(mpz_t*));

  for (int j=0 ; j<3 ; j++) {
    X[j]=calloc(2,sizeof(mpz_t));
    mpz_inits(X[j][0],X[j][1],NULL);
  }

  //Offset generation
  gen_alea(offset,rand());
  if(mpz_odd_p(offset)==0) mpz_add_ui(offset,offset,1); //Set the offset as odd change the signal bit when xoring with it

  //Inputs key generation
  gen_input_key(kA,kB,offset,rand()); //IDEE : utiliser CPU cycles

  //Translation table computation
  mpz_set_ui(X[0][0],0);
  for (int i=0 ; i<PARAM_L ; i++) {
    gate_xor_garb(X[1],kA[i][0],X[0][0],offset);
    gate_xor_garb(X[2],kB[i][0],X[0][0],offset);
    gate_and_garb(A,ct_AND[i],X[1],X[2],offset);
    if (PARAM_INEQ % 4 > 1) gate_xor_garb(X[0],A,kB[i][0],offset);
    if (PARAM_INEQ % 4 < 2) gate_xor_garb(X[0],A,kA[i][0],offset);
    }
  gate_xor_garb(X[1],kB[PARAM_L][0],X[0][0],offset);
  gate_xor_garb(trans_table,kA[PARAM_L][0],X[1][0],offset);
  H(trans_table[0],trans_table[0]);
  H(trans_table[1],trans_table[1]);
  //memory release
  for (int j=0 ; j<3 ; j++) {
    mpz_clears(X[j][0],X[j][1],NULL);
    free(X[j]);
  }
  mpz_clears(offset, A, NULL);
  free(X);
}

/**
  * \fn int cmp_Bob_eval(mpz_t * Alice_input_keys, mpz_t * Bob_input_keys, mpz_t ** ct_AND, mpz_t * trans_table)
  * \brief This function evaluates a comparison garbled circuit

  * \param[in] Alice_input_keys   mpz_t array representing Alice's inputs keys
  * \param[in] Bob_input_keys     mpz_t array representing Bob's input keys
  * \param[in] ct_AND             mpz_t double array representing the AND gates ciphertexts
  * \param[in] trans_table        mpz_t array representing the translation table

  * \return 0 if the outputted key matches the key associated to 0 in the translation table
  * \return 1 if the outputted key matches the key associated to 1 in the translation table
  * \return -1 if the key does not match any key.
*/
int cmp_Bob_eval(mpz_t * Alice_input_keys, mpz_t * Bob_input_keys, mpz_t ** ct_AND, mpz_t * trans_table) {
  mpz_t temp, output;
  mpz_inits(temp, output, NULL);
  mpz_set_ui(output,0);

  for (int i=0 ; i<PARAM_L ; i++) {
    gate_xor_eval(temp,Alice_input_keys[i],output);
    gate_xor_eval(output,Bob_input_keys[i],output);
    gate_and_eval(output,temp,output,ct_AND[i]);
    if (PARAM_INEQ % 4 > 1) gate_xor_eval(output,output,Bob_input_keys[i]);
    if (PARAM_INEQ % 4 < 2) gate_xor_eval(output,output,Alice_input_keys[i]);
   }
  gate_xor_eval(output,output,Bob_input_keys[PARAM_L]);
	gate_xor_eval(output,output,Alice_input_keys[PARAM_L]);
  H(output,output);
  if (mpz_cmp(output,trans_table[0])==0) {
    mpz_clears(temp, output, NULL);
    return 0;
  } else {
    if (mpz_cmp(output,trans_table[1])==0) {
      mpz_clears(temp, output, NULL);
      return 1;
    }
  }
  mpz_clears(temp, output, NULL);
  return -1;
}

/**
  * \fn void gate_and_garb(mpz_t  A_out, mpz_t* ct_AND, mpz_t* A1, mpz_t* A2,mpz_t offset)
  * \brief This function garbles an AND gate

  * \param[out] A_out   mpz_t  representing the output key
  * \param[out] ct_AND  mpz_t double array representing the AND gates ciphertexts

  * \param[in] A1       mpz_t double array representing the Alice's input keys
  * \param[in] A2       mpz_t double array representing the Bob's input keys
  * \param[in] offset   mpz_t representing the offset value for FreeXOR
*/
void gate_and_garb(mpz_t  A_out, mpz_t* ct_AND, mpz_t* A1, mpz_t* A2,mpz_t offset) {

  mpz_t kC_E,kC_G;
  mpz_t * hash_A1=calloc(2,sizeof(mpz_t)),* hash_A2=calloc(2,sizeof(mpz_t));
  for (int i=0;i<2;i++)  mpz_inits(hash_A1[i],hash_A2[i],NULL) ;

  mpz_inits(kC_E,kC_G,NULL);

  int pa=mpz_tstbit(A1[0],0),pb=mpz_tstbit(A2[0],0); //Values corresponding to signal bits
  H(hash_A1[0],A1[0]);
  H(hash_A1[1],A1[1]);
  H(hash_A2[0],A2[0]);
  H(hash_A2[1],A2[1]);


  //First Half Gate
  mpz_xor(ct_AND[0],hash_A1[0],hash_A1[1]);
  if (pb==1) mpz_xor(ct_AND[0], ct_AND[0], offset);

  mpz_set(kC_G, hash_A1[0]);
  if (pa==1) mpz_xor(kC_G, kC_G, ct_AND[0]);


  //Second Half Gate
  mpz_xor(ct_AND[1],hash_A2[0],hash_A2[1]);
  mpz_xor(ct_AND[1], ct_AND[1], A1[0]);

  mpz_set(kC_E,hash_A2[0]);
  if (pb==1) {
    mpz_xor(kC_E,kC_E,ct_AND[1]);
    mpz_xor(kC_E,kC_E,A1[0]);
  }

  //XORing gives the output key
  mpz_xor(A_out,kC_E,kC_G);

  mpz_clears(kC_G,kC_E,hash_A1[0], hash_A1[1], hash_A2[0], hash_A2[1] ,NULL);
  free(hash_A1);
  free(hash_A2);
}

/**
  * \fn void gate_xor_garb(mpz_t* X_out, mpz_t X1, mpz_t X2, mpz_t offset)
  * \brief This function garbles an XOR gate

  * \param[out] X_out   mpz_t array representing the garbled XOR gates
  * \param[in]  X1       mpz_t representing the first input to the gate
  * \param[in]  X2       mpz_t representing the second input to the gate
  * \param[in]  offset   mpz_t representing the offset value for FreeXOR
*/
void gate_xor_garb(mpz_t* X_out, mpz_t X1, mpz_t X2, mpz_t offset) {
  mpz_xor(X_out[0],X1,X2);
  mpz_xor(X_out[1],X_out[0],offset);
}

/**
  * \fn void gate_and_eval(mpz_t A_out, mpz_t A1, mpz_t A2, mpz_t * AND_ct)
  * \brief This function evaluates an AND gate

  * \param[out] A_out   mpz_t representing the garbled AND gate

  * \param[in]  A1      mpz_t representing the first input to the gate
  * \param[in]  A2      mpz_t representing the second input to the gate
  * \param[in]  AND_ct  mpz_t array representing the gate ciphertexts
*/
void gate_and_eval(mpz_t A_out, mpz_t A1, mpz_t A2, mpz_t * AND_ct) {

	mpz_t hash_A1, hash_A2, kG, kE; //kG is the garbler ciphertext while kE is the Evaluator's one
	mpz_inits(hash_A1, hash_A2, kG, kE,NULL);

 // We compute the least significant bit of each input
	int sa=mpz_tstbit(A1,0), sb=mpz_tstbit(A2,0);

	//First half gate evaluation
 	H(hash_A1,A1);
 	H(hash_A2,A2);

 	mpz_set(kG,hash_A1);
 	if (sa==1) mpz_xor(kG,kG,AND_ct[0]);

	//Second half gate evaluation
  mpz_set(kE,hash_A2);
  if (sb==1) {
 	 mpz_xor(kE,kE,AND_ct[1]);
 	 mpz_xor(kE, kE ,A1);
  }

	//XOR the two halves to make a whole
 	mpz_xor(A_out, kG,kE);
  mpz_clears(hash_A1, hash_A2, kG, kE,NULL);

}

/**
  * \fn void gate_xor_eval(mpz_t X_out, mpz_t X1, mpz_t X2)
  * \brief This function evaluates an XOR gate

  * \param[out] X_out  mpz_t representing the outputted key

  * \param[in]  X1     mpz_t representing the first input to the gate
  * \param[in]  X2     mpz_t representing the second input to the gate
*/
void gate_xor_eval(mpz_t X_out, mpz_t X1, mpz_t X2) {
  mpz_xor(X_out, X1, X2);
}
