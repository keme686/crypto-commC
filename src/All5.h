#ifndef _ALL5_H
#define _ALL5_H

#include "Maj5.h"


void all5_encrypt(uint8_t* msg, uint64_t key, uint8_t* cipher, int length);
void all5_decrypt(uint8_t* msg, uint64_t key, uint8_t* cipher, int length);

#endif

