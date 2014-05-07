#ifndef _SPONGE_H_
#define _SPONGE_H_

#include "bunny24.h"
#include <stdint.h>
#include<stdlib.h>

//void sponge_hash(uint64_t msg, unsigned int n, uint64_t key, uint64_t* output);
void sponge_hash(uint8_t* msg, unsigned int n,  uint8_t* output);

#endif
