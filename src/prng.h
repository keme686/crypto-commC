#ifndef _PRNG_H_
#define _PRNG_H_

#include "bunny24.h"
#include "All5.h"
#include "Maj5.h"
#include "lfsr.h"
#include "A5_1.h"

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<stdint.h>

#include <openssl/bio.h>
#include <openssl/bn.h>

void getRandom(uint8_t* num, int length); //length = number of bits
void fprng(uint8_t* num, int length);
void sprng(uint8_t* num, int length);
void primeNum(uint8_t* num, int length);

#endif
