#ifndef _MAJ5_H
#define _MAJ5_H

#include "A5_1.h"

void init_Maj5(uint64_t* registers, uint64_t* state, uint64_t* taps, int* deg);

void maj5_encrypt(uint8_t* msg, uint64_t key, uint8_t* cipher, int length);
void maj5_decrypt(uint8_t* msg, uint64_t key, uint8_t* cipher, int length);


#endif

