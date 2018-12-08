/**
  * \file twisted_edwards_curves.h
  * \brief functions for computation on Twisted Edwards curves
*/

#ifndef TWISTED_EDWARDS_CURVES_H
#define TWISTED_EDWARDS_CURVES_H

#include <gmp.h>
#include "auxiliary_functions.h"

/**
  * \typedef ted_point
  * \brief Structure of a point of an Twisted Edwards Curve
  */
typedef struct ted_point{
  mpz_t x ; /**< First coordinate of the point */
  mpz_t y ; /**< Second coordinate of the point */
} ted_point;


ted_point* ted_point_init();
void ted_point_clear(ted_point* P);

void ted_point_set_str(ted_point *P, char* x, char* y, int base);
void ted_point_set(ted_point* P, ted_point* Q);

int ted_curve_in(ted_point * P);

void ted_point_opp(ted_point * opP , ted_point * P);

void ted_point_add(ted_point* R, ted_point* P1, ted_point* P2);
void ted_point_mult(ted_point* out, ted_point* P, mpz_t s);
void ted_point_double(ted_point * R, ted_point * P);

void ted_encode( uint8_t * enc_P , ted_point * P);
void ted_decode(ted_point * P, uint8_t * enc);

#endif
