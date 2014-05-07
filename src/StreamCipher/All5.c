#include "../All5.h"
#include<stdio.h>
/* 
 *  shift the all bits registers  - overwrites on state values
 *
 * registers = array of registers
 * num_reg   = number of registers
 * state     = array of state vector
 * deg       = array of integers indicating degree of of each register
 * tap       = array of clock positions
*/
uint8_t all_shift(uint64_t* registers, int num_reg, uint64_t* state, int* deg, uint64_t* tap)
{  
  int i;    
  uint8_t bits[5];
  uint8_t s[100];
  //shif the state with majority bit
  for(i=0; i<num_reg; i++)
  {
   bits[i] = (state[i] & 1)?1:0;
   const int l=1;   
   lfsr(&registers[i],&state[i],deg[i],l,s);      
  }  
   return (bits[0] & bits[3]) ^ (bits[1] & bits[2])
    ^ (bits[1] & bits[4]) ^ (bits[2] & bits[3]);
}


/*
 *    All5
 * key      = 64 bit key
 * output   = output vector
 * out_bits = number of output bits
*/
void all5(uint64_t* registers, uint64_t* state, uint64_t* taps, int* deg, uint8_t* output, int out_bits)
{
 int i;
 // Step 5 - n IRREGULAR CLOCKS WITH OUTPUT
 for(i=0; i<out_bits; i++)
 {  
  int out=all_shift(registers, 5, state, deg, taps);  
  //output[i]=out?'1':'0';
  int a = out?1:0;
  *output ^= a<<(7- (i%8));
 }  
}

void all5_encrypt(uint8_t* msg, uint64_t key, uint8_t* cipher, int length)
{
 uint64_t registers[5]={0},state[5]={0},taps[5]={0};
 int deg[5]={0};
 uint64_t frame = 0x2C80000000000000;
 int len = length/8 + (length%8>0?1:0);
 uint8_t ciphers[len];
 init_Maj5(registers, state, taps, deg);
 
 // Step 1 - Key Injection
 keyloading(key,registers, 5, state, deg, 64);

 // Step 2 - frame initialization
 keyloading(frame,registers, 5, state, deg, 22);

 // Step 4 - 100 IRREGULAR CLOCKS WITH NO OUTPUT
 int i; 
 for(i=0; i<100; i++)
 { 
  all_shift(registers, 5, state, deg, taps);
 }

 for(i=0; i<length/8; i++)
 {
   ciphers[i] = 0;
 }
 if(length%8>0){
   ciphers[i]=0;
 }
  for(i=0; i<length/8; i++)
 {
  all5(registers, state, taps, deg, &ciphers[i], 8);
  ciphers[i] ^= msg[i];
 }
 if(length%8>0){
  all5(registers, state, taps, deg, &ciphers[i], length%8);
  ciphers[i] ^= msg[i];   
  }

  for(i=0; i<length/8; i++)
 {
   cipher[i] = ciphers[i] ;
 }
 if(length%8>0){
   cipher[i] = ciphers[i] ;
 }
  
}
void all5_decrypt(uint8_t* msg, uint64_t key, uint8_t* cipher, int length)
{
  all5_encrypt(msg, key, cipher, length);
}
