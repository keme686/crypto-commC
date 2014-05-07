#ifndef _BUNNY24_H_
#define _BUNNY24_H_

#include <stdint.h>
#include<stdlib.h>
#include "field.h"
void BunnyTn_enc(uint64_t msg, uint64_t key,uint64_t* cipher);
void BunnyTn_encrypt(uint8_t* msgs, unsigned int num_bytes, uint64_t key, uint8_t* cipher);
void BunnyTn_decrypt(uint8_t* cipher, unsigned int num_bytes, uint64_t key, uint8_t* plain);  //n = number of bytes


//void BunnyTncbc_enc(uint64_t* msg, unsigned int n, uint64_t key, uint64_t iv, uint64_t* output);
//void BunnyTncbc_decr(uint64_t* msg, unsigned int n, uint64_t key, uint64_t iv, uint64_t* output);

void BunnyTncbc_encrypt(uint8_t* msgs,uint8_t n, uint8_t* key, uint64_t iv, uint8_t* cipher); //n = number of bytes
void Bunnycbc_decrypt(uint8_t* msgs,uint8_t n, uint8_t* key, uint64_t iv, uint8_t* cipher);  //n = number of bytes

#endif
