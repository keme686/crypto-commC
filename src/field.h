#ifndef _FIELD_H_
#define _FIELD_H_

#include <stdint.h>
#include <stdlib.h>

uint8_t field_add(uint8_t a, uint8_t b); //6 = number of bits
uint8_t field_rotate(uint8_t v, uint8_t t); //6 = number of bits, t = number of bits to rotate
uint8_t field_product(uint8_t a, uint8_t b); //6 = number of bits , polynomial x^6 + x^4 + x^3 + x + 1 =  0x5B

#endif
