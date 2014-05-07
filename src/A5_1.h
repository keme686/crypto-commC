#ifndef _A51_H
#define _A51_H

#include "lfsr.h"

static uint64_t FRAME = 0x2C80000000000000;


void keyloading(uint64_t key,uint64_t* registers, int num_reg, uint64_t* state, int* deg, int out_len);
void maj_shift(uint64_t* registers, int num_reg, uint64_t* state, int* deg, uint64_t* tap);

void a51_encrypt(uint8_t* msg, uint64_t key, uint8_t* cipher, int length);
void a51_decrypt(uint8_t* msg, uint64_t key, uint8_t* cipher, int length);


#endif


