/**
  * \file cmp_steps.h
  * \brief Functions used for the computation of the comparison
*/

#ifndef COMPARISON_H
#define COMPARISON_H

#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <time.h>

#include "auxiliary_functions.h"
#include "gate_functions.h"
#include "oblivious_transfer.h"



void step1_init(mpz_t Alice, mpz_t y, uint8_t * Alice_input);
void step1_clear(uint8_t * ct_Alice, uint8_t * sen_y, mpz_t Alice, mpz_t y);
void cmp_Alice_step1(Alice_struct * Alice, OT_sender * Alice_OT, uint8_t * Alice_input);

void step2_init(mpz_t * x, mpz_t mpz_Bob, mpz_t ct_Alice, mpz_t rho, mpz_t ct_gamma, Bob_struct * Bob, uint8_t * Bob_input);
void step2_clear(Bob_struct * Bob, OT_receiver * Bob_OT, mpz_t * x, mpz_t mpz_Bob, mpz_t ct_Alice, mpz_t rho, mpz_t ct_gamma);
void cmp_Bob_step2(Bob_struct * Bob, OT_receiver * Bob_OT, uint8_t * Bob_input);

void step3_init(mpz_t gamma, mpz_t ct_gamma, mpz_t y, mpz_t ** keys, mpz_t ** kA, mpz_t ** kB, mpz_t ** ct_AND, mpz_t * input_keys, mpz_t * trans_table, Alice_struct * Alice, OT_sender * Alice_OT);
void step3_clear(Alice_struct * Alice, OT_sender * Alice_OT, mpz_t gamma, mpz_t ct_gamma, mpz_t y, mpz_t ** keys, mpz_t ** kA, mpz_t ** kB, mpz_t ** ct_AND, mpz_t * input_keys, mpz_t * trans_table);
int cmp_Alice_step3(Alice_struct * Alice, OT_sender * Alice_OT);

void step4_init(mpz_t rho, mpz_t * x, mpz_t * trans_table, mpz_t ** ct_AND, mpz_t * Bob_keys, mpz_t * Alice_keys, mpz_t ** keys, Bob_struct * Bob, OT_receiver * Bob_OT);
void step4_clear(Bob_struct * Bob, OT_receiver * Bob_OT, mpz_t rho, mpz_t * x, mpz_t * trans_table, mpz_t ** ct_AND, mpz_t * Bob_keys, mpz_t * Alice_keys, mpz_t ** keys);
int cmp_Bob_step4(Bob_struct * Bob , OT_receiver * Bob_OT);

#endif
