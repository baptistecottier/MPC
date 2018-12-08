/**
 * \file twisted_edwards_curves.c
 * \brief Implementation of edward_curves.h
 */

#include <stdio.h>
#include <stdlib.h>

#include "twisted_edwards_curves.h"


/**
  * \fn ted_point* ted_point_init()
  * \brief This function initializes a twisted Edwards curve  point

  * \return P a ted_point
*/
ted_point* ted_point_init() {
  ted_point* P = (ted_point*) malloc(sizeof(ted_point));
  mpz_inits(P->x,P->y,NULL);
  return P;
}

/**
  * \fn void ted_point_clear(ted_point* P)
  * \brief This function clears the allocate memory for ted_point

  * \param[in] P the ted_point to free
*/
void ted_point_clear(ted_point* P) {
  mpz_clears(P->x,P->y,NULL);
  free(P);
}

/**
  * \fn void ted_point_set_str(ted_point* P,   char * x,   char * y, int base)
  * \brief This function sets a point from given strings and bases

  * \param[out] P    ted_point representing the point to set

  * \param[in] x    char array representing the abscissa value of the point
  * \param[in] y    char array representing the ordinate value of the point
  * \param[in] base int representing the base on which the point is setted
*/
void ted_point_set_str(ted_point* P, char * x, char * y, int base) {
  mpz_set_str(P->x,x,base);
  mpz_set_str(P->y,y,base);
}

/**
  * \fn void ted_point_set(ted_point* P, ted_point* Q)
  * \brief This function sets a ted_point equal to a second ted_point

  * \param[out] P   ted_point repre the point to set
  * \param[in]  Q   ted_point representing the refererent point
*/
void ted_point_set(ted_point* P, ted_point* Q) {
  mpz_set(P->x, Q->x);
  mpz_set(P->y, Q->y);
}

/**
  * \fn void ted_point_opp(ted_point * opP, ted_point * P)
  * \brief This function return the opposite value of a point

  * \param[out] opP representing the opposite point
  * \param[in]  P   ted_point representing point to oppose
*/
void ted_point_opp(ted_point * opP , ted_point * P) {
  mpz_set(opP->y,P->y);
  mpz_mul_si(opP->x,P->x,-1);
}

/**
  * \fn int ted_curve_in(ted_point * P)
  * \brief This function tests if a given point belong to the curve.

  * \param[in] P ted_point representing the point to test

  * \return 1 if the test succeed
  * \return 0 if the test failed
  */
int ted_curve_in(ted_point * P) {

  mpz_t TED_C_Q, TED_C_D,R_x,R_y,temp1,temp2,temp3 ;
  mpz_inits(TED_C_Q, TED_C_D, R_x,R_y,temp1,temp2,temp3, NULL);
  mpz_set_str(TED_C_Q,TED_CURVE_Q,16);
  mpz_set_str(TED_C_D,TED_CURVE_D,16);

  //copy of the inputs values to don't modify them
  mpz_set(R_x,P->x);
  mpz_set(R_y,P->y);

  //computation of -x²+y²
  mpz_powm_ui(temp1,R_x,2,TED_C_Q);
  mpz_set(temp3,temp1);
  mpz_mul_si(temp1,temp1,-1);
  mpz_mod(R_x,R_x,TED_C_Q);
  mpz_mul(temp2,R_y,R_y);
  mpz_mod(temp2,temp2,TED_C_Q);
  mpz_add(R_x,temp2,temp1);
  mpz_mod(R_x,R_x,TED_C_Q);

  //computation of 1 + d * x² * y²
  mpz_mul(R_y,temp3,temp2);
  mpz_mod(R_y,R_y,TED_C_Q);
  mpz_mul(R_y,R_y,TED_C_D);
  mpz_mod(R_y,R_y,TED_C_Q);
  mpz_add_ui(R_y,R_y,1);
  mpz_mod(R_y,R_y,TED_C_Q);

  //comparison
  if (mpz_cmp(R_x,R_y)==0) {
    mpz_clears(TED_C_Q, TED_C_D, R_x,R_y,temp1,temp2,temp3, NULL);
    return 1;
  }
  mpz_clears(TED_C_Q, TED_C_D, R_x,R_y,temp1,temp2,temp3, NULL);
  return 0;
}

/**
  * \fn int ted_point_add(ted_point* R, ted_point* P1, ted_point* P2)
  * \brief This function returns the sum of two points

  * \param[out] R ted_point representing the sum of the points

  * \param[in] P1 ted_point representing the first point to sum
  * \param[in] P2 ted_point representing the second point to sum
*/
void ted_point_add(ted_point* R, ted_point* P1, ted_point* P2) {

  mpz_t TED_C_Q, TED_C_D, den_x,den_y;
  mpz_inits(TED_C_Q, TED_C_D, den_x,den_y, NULL);
  mpz_set_str(TED_C_Q,TED_CURVE_Q,16);
  mpz_set_str(TED_C_D,TED_CURVE_D,16);

  mpz_mul(R->x,P1->x,P2->y); // x1 * y2
  mpz_mod(R->x,R->x,TED_C_Q);
  mpz_mul(R->y,P2->x,P1->y); // x2 * y1
  mpz_mod(R->y,R->y,TED_C_Q);
  mpz_mul(den_x,R->x,R->y); // x1*x2*y1*y2
  mpz_mul(den_x,den_x,TED_C_D); // d * x1 * x2 * y1 * y2
  mpz_mod(den_x,den_x,TED_C_Q);
  mpz_set(den_y,den_x); //  d * x1 * x2 * y1 * y2

  mpz_add_ui(den_x,den_x,1); //1 + d * x1 * x2 * y1 * y2
  mpz_invert(den_x,den_x,TED_C_Q); // (1 + d * x1 * x2 * y1 * y2)^-1
  mpz_add(R->x,R->x,R->y);// x1 * y2 + x2 * y1
  mpz_mod(R->x,R->x,TED_C_Q);
  mpz_mul(R->x,R->x,den_x); //(x1 * y2 + x2 * y1)*(1 + d * x1 * x2 * y1 * y2)^-1
  mpz_mod(R->x,R->x,TED_C_Q);

  mpz_ui_sub(den_y,1,den_y);// (1 - d * x1 * x2 * y1 * y2)
  mpz_invert(den_y,den_y,TED_C_Q); // (1 - d * x1 * x2 * y1 * y2)^-1
  mpz_mul(den_x,P1->x,P2->x);// x1 * x2
  mpz_mul(R->y,P1->y,P2->y); // y1 * y2
  mpz_add(R->y,den_x,R->y); // x1 * x2 + y1 * y2
  mpz_mul(R->y,R->y,den_y); // (x1 * x2 + y1 * y2) * (1 - d * x1 * x2 * y1 * y2)^-1
  mpz_mod(R->y,R->y,TED_C_Q);

  mpz_clears(TED_C_Q, TED_C_D, den_x,den_y, NULL);
}

/**
  * \fn void ted_point_double(ted_point * R, ted_point * P)
  * \brief This function computes the double of a ted_point

  * \param[out] R ted_point representing the computed value

  * \param[in] P ted_point representing the point to double
*/
void ted_point_double(ted_point * R, ted_point * P) {

  mpz_t TED_C_Q, TED_C_D, x2, y2, den_x,den_y;
  mpz_inits(TED_C_Q, TED_C_D, x2, y2, den_x,den_y, NULL);
  mpz_set_str(TED_C_Q,TED_CURVE_Q,16);
  mpz_set_str(TED_C_D,TED_CURVE_D,16);

  mpz_powm_ui(x2,P->x,2,TED_C_Q);
  mpz_powm_ui(y2,P->y,2,TED_C_Q);

  mpz_mul(R->x,P->x,P->y); //rx = x * y
  mpz_mul_ui(R->x,R->x,2); //rx = 2 * x * y mod q = x * y
  mpz_mod(R->x,R->x,TED_C_Q); //rx = 2xy mod q = 2xy

  mpz_sub(den_x,y2,x2); // den_x = y2 - x2
  mpz_mod(den_x,den_x,TED_C_Q);
  mpz_invert(R->y,den_x,TED_C_Q); //ry = (y2-x2)^(-1)
  mpz_mul(R->x,R->x,R->y); // 2xy * (y2-x2)^(-1)
  mpz_mod(R->x,R->x,TED_C_Q);

  mpz_add(R->y,y2,x2);
  mpz_mod(R->y,R->y,TED_C_Q);
  mpz_ui_sub(den_y,2,den_x);
  mpz_mod(den_y,den_y,TED_C_Q);
  mpz_invert(den_y,den_y,TED_C_Q);
  mpz_mul(R->y,R->y,den_y);
  mpz_mod(R->y,R->y,TED_C_Q);

  mpz_clears(TED_C_Q, TED_C_D, x2, y2, den_x,den_y, NULL);
}

/**
  * \fn int ted_point_mult(ted_point * out, ted_point * P, mpz_t s)
  * \brief This function computes the product of a scalar and a point

  * \param[out] out ted_point representing the computed point
  * \param[in] P    ted_point representing the ted_point to multiply
  * \param[in] s    mpz_t representing the scalar value

  * \return -1 if P is not on the twisted Edwards curve
  * \return 0  if the computation succeed
*/
void ted_point_mult(ted_point * out, ted_point * P, mpz_t s) {

  ted_point* T=ted_point_init();
  mpz_set_ui(T->y,1); //Initialisation of T as neutral element
  for (int i=mpz_sizeinbase(s,2)-1 ; i>=0 ; i--) {
    ted_point_double(out,T);
    mpz_set(T->x,out->x);
    mpz_set(T->y,out->y);
    if (mpz_tstbit(s,i)==1) {
      ted_point_add(out,P,T);
      mpz_set(T->x,out->x);
      mpz_set(T->y,out->y);
    }
  }
  ted_point_clear(T);
}

/**
  * \fn void ted_encode(uint8_t * enc, ted_point * P)
  * \brief This function encodes a ted_point into a 256 bits

  * \param[out] enc the encoding of a ted_point

  * \param[in] P the ted_point to encode
*/
void ted_encode(uint8_t * enc, ted_point * P) {
  mpz_t temp, a,z,e,r;
  mpz_inits(temp,a,z,e,r,NULL);

  mpz_set(temp,P->y);
  mpz_mul_ui(temp,temp,2);
  mpz_add_ui(temp,temp,mpz_tstbit(P->x,0));
  mpz_export(enc,NULL,-1,1,0,0,temp);
  mpz_clears(temp,a,z,e,r,NULL);
}

/**
  * \fn void ted_decode(ted_point * P, uint8_t * enc)
  * \brief This function retrieves a ted_point from an encoded point

  * \param[out] P the decoded ted_point
  
  * \param[in] enc the encoded ted_point
*/
void ted_decode(ted_point * P,   uint8_t * enc) {

  mpz_t TED_C_Q, TED_C_D, r,a,y_square,E ;
  mpz_inits(TED_C_Q, TED_C_D, r,a,y_square,E,NULL);
  mpz_set_str(TED_C_Q,TED_CURVE_Q,16);
  mpz_set_str(TED_C_D,TED_CURVE_D,16);

  mpz_sub_ui(a,TED_C_Q,1);
  mpz_cdiv_q_ui(a,a,2);
  mpz_import(E,1,-1,32,0,0,enc);
  int sign = mpz_tstbit(E,0);
  mpz_sub_ui(E,E,mpz_tstbit(E,0));
  mpz_cdiv_q_ui(P->y,E,2);
  mpz_powm_ui(y_square,P->y,2,TED_C_Q);

  mpz_sub_ui(r,y_square,1); // y²-1

  mpz_mul(y_square,y_square,TED_C_D);
  mpz_add_ui(y_square,y_square,1);
  mpz_mod(y_square,y_square,TED_C_Q);

  mpz_invert(y_square,y_square,TED_C_Q);
  mpz_mul(r,r,y_square);
  mpz_mod(r,r,TED_C_Q);
  mpz_quad_res(P->x,r,TED_C_Q);

  if (mpz_tstbit(P->x,0)!=sign) mpz_mul_si(P->x,P->x,-1);
  mpz_mod(P->x,P->x,TED_C_Q);

  mpz_clears(TED_C_Q, TED_C_D, r,a,y_square,E,NULL);

}
