
#ifndef API_H
#define API_H

int cmp_Alice_step1(uint8_t * Alice_input, Alice_struct * Alice, OT_sender * Alice_OT);
int cmp_Bob_step2(uint8_t *  Bob_input, Bob_struct * Bob, OT_receiver * Bob_OT);
int cmp_Alice_step3(Alice_struct * Alice, OT_sender * Alice_OT);
int cmp_Bob_step4(Bob_struct * Bob , OT_receiver * Bob_OT);

#endif