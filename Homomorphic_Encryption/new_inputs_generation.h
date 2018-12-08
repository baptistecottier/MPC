/**
  * \typedef Alice_struct
  * \brief Structure for Alice's variables
  */
typedef struct Alice_struct {
  mpz_t  ct_Alice ; /**< Alice's input ciphertext */
  mpz_t  ct_gamma ; /**< Alice's new input ciphertext */
} Alice_struct ;

/**
  * \typedef Bob_struct
  * \brief Structure for Bob's variables
  */
typedef struct Bob_struct {
 mpz_t rho ; /**< Bob's new input */
 mpz_t ct_Alice ; /**< Alice'es input ciphertext */
 mpz_t ct_gamma ; /**< Alice's new input ciphertext */

} Bob_struct ;
