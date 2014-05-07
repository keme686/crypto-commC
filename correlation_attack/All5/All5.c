#include "../All5.h"

void printBin(uint64_t x, int d)
{
  char buffer[d+1];
  int index=0;
  while(d>0)
  {
   if(x&1)
     buffer[index++]='1';
   else 
     buffer[index++]='0';
   x>>=1;
   d--;
  }
  while(index>0)
   printf("%c", buffer[--index]);
   return;
}

/*  
 * Maj5 and all5 initialization - initialization will be overwriten on the arrays passed
 *
 * registers = array of registers to be initialized
 * taps      = array of clock positions to be initialized
 * deg       = array of integer to initialize degree of each registers
*/ 
void init_all5(uint64_t* registers, uint64_t* state, int* deg)
{
 registers[0]=0x7;
 registers[1] =0xD;
 registers[2]= 0x19; 
 registers[3] =0xC1;
 registers[4]= 0x1C3; 

 deg[0]=2;
 deg[1]=3;
 deg[2]=4;
 deg[3]=7;
 deg[4]=8;

 state[0] =0x2;
 state[1] =0x5;
 state[2] =0x8;
 state[3] =0x2D;
 state[4] =0xAF;
}


/* 
 *  shift the all bits registers  - overwrites on state values
 *
 * registers = array of registers
 * num_reg   = number of registers
 * state     = array of state vector
 * deg       = array of integers indicating degree of of each register
 * tap       = array of clock positions
*/
uint8_t all_shift(uint64_t* registers, int num_reg, uint64_t* state, int* deg)
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
  return ((bits[0] & bits[1] & bits[2] & bits[3] & bits[4]) ^ (bits[3] & bits[4]) ^ bits[4]); //f = x1.x2.x3.x4.x5 + x4.x5 + x5 
}

/*
 *    All5
 * key      = 64 bit key
 * output   = output vector
 * out_bits = number of output bits
*/
void all5(uint64_t* registers, uint64_t* state, int* deg, uint8_t* output, int out_bits)
{
 int i;
 // Step 5 - n IRREGULAR CLOCKS WITH OUTPUT
 for(i=0; i<out_bits; i++)
 {  
  int out=all_shift(registers, 5, state, deg);  
  int a = out?1:0;
  *output ^= a<<(7- (i%8));
 }   
}

void all5_encrypt(uint8_t* msg, uint64_t key, uint8_t* cipher, int length)
{
 uint64_t registers[5],state[5];
 int deg[5];
 uint64_t frame = 0x2C80000000000000;
 init_all5(registers, state, deg);
 // Step 1 - Key Injection
 keyloading(key,registers, 5, state, deg, 64);
 // Step 2 - frame initialization
 keyloading(frame,registers, 5, state, deg, 22);
 // Step 4 - 100 IRREGULAR CLOCKS WITH NO OUTPUT
 int i; 
 for(i=0; i<100; i++)
 { 
  all_shift(registers, 5, state, deg);
 }
 
  for(i=0; i<length/8; i++)
 {
  all5(registers, state, deg, &cipher[i], 8);
  cipher[i] ^= msg[i];
 }
 if(length%8>0){
  all5(registers, state, deg, &cipher[i], length%8);
  cipher[i] ^= msg[i];   
  }

}
void all5_decrypt(uint8_t* msg, uint64_t key, uint8_t* cipher, int length)
{
  all5_encrypt(msg, key, cipher, length);
}
