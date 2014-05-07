#include "field.h"

//add two field vectors
uint8_t field_add(uint8_t a, uint8_t b)
{
 uint8_t c = a^b;
 return c;
}

//right rotation
uint8_t field_rotate(uint8_t v, uint8_t t)
{
  uint8_t p = 8-t;
  uint8_t x = v>>t;
  uint8_t y = v<<p;
  y>>=2;
  uint8_t z = x^y;
  return z;
}

//a and b field product with primitive polynomial poly
uint8_t field_product(uint8_t a, uint8_t b)
{
 uint8_t pr =0;
 uint8_t i,c;
 uint8_t poly = 0x5B;//polynomial x^6 + x^4 + x^3 + x + 1
 for(i =0; i<6; i++)
 {
  if(b & 1)
     pr ^= a;
   c = a & (1<<(6-1)); //detect last element
   a <<= 1; 
   if(c)
     a ^= poly;
   b >>= 1;
 }
 return pr;
}

