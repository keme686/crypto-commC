#ifndef _ALL5_H
#define _ALL5_H

#include "Maj5.h"
#include<stdio.h>

void all5_encrypt(uint8_t* msg, uint64_t key, uint8_t* cipher, int length);
void all5_decrypt(uint8_t* msg, uint64_t key, uint8_t* cipher, int length);
void all5(uint64_t* registers, uint64_t* state, int* deg, uint8_t* output, int out_bits);
#endif

