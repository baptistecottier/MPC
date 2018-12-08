/**
  * \typedef Alice_struct
  * \brief Structure for Alice's variables
  */
typedef struct HE_cmp_Alice {
  mpz_t ct_Alice ; /**< Alice's input ciphertext */
  mpz_t ct_gamma ; /**< Alice's new input ciphertext */
  mpz_t gamma ;
  mpz_t * ct_bits ;
} Alice_struct ;

/**
  * \typedef Bob_struct
  * \brief Structure for Bob's variables
  */
typedef struct HE_cmp_Bob {
  mpz_t rho ; /**< Bob's new input */
  mpz_t ct_Alice ; /**< Alice'es input ciphertext */
  mpz_t ct_gamma ; /**< Alice's new input ciphertext */
  mpz_t r ;
  mpz_t * ct_e ;
  mpz_t * ct_ep ;
} Bob_struct ;
