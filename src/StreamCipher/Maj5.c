#include "../Maj5.h"

/*  
 * Maj5 and all5 initialization - initialization will be overwriten on the arrays passed
 *
 * registers = array of registers to be initialized
 * taps      = array of clock positions to be initialized
 * deg       = array of integer to initialize degree of each registers
*/ 
void init_Maj5(uint64_t* registers, uint64_t* state, uint64_t* taps, int* deg)
{
 registers[0]=0x80027;
 registers[1] =0x400003;
 registers[2]= 0x808007; 
 registers[3] =0xA01;
 registers[4]= 0x3601; 

 taps[0]=0x400; //2^9,
 taps[1] =0x800;  // 2^11
 taps[2] =0x1000; // 2^11
 taps[3] =0x40;  //2^5
 taps[4] =0x40;  //2^7

 deg[0]=19;
 deg[1]=22;
 deg[2]=23;
 deg[3]=11;
 deg[4]=13;

 state[0] =0;
 state[1] =0;
 state[2] =0;
 state[3] =0;
 state[4] =0;
}


/*
 *    MAJ5
 * key      = 64 bit key
 * output   = output vector
 * out_bits = number of output bits
*/
void maj5(uint64_t* registers, uint64_t* state, uint64_t* taps, int* deg, uint8_t* output, int out_bits)
{
 int i,j;
 //Step 5 - n IRREGULAR CLOCKS WITH OUTPUT
 for(i=0; i<out_bits; i++)
 {
  int out=0;
  for(j=0; j<5; j++)
  {
   uint64_t temp = state[j] ; 
   out ^= temp&((uint64_t)1)?1:0;   
  }
  int a = out?1:0;
  *output ^= a<<(7- (i%8));
  maj_shift(registers, 5, state, deg, taps);
 }
}


void maj5_encrypt(uint8_t* msg, uint64_t key, uint8_t* cipher, int length)
{
 uint64_t registers[5],state[5],taps[5];
 int deg[5];
 uint64_t frame = 0x2C80000000000000;
 //Step 1 - Initialization
 init_Maj5(registers, state, taps, deg);

 //Step 2 - Key Injection
 keyloading(key,registers, 5, state, deg, 64);
 
// Step 3 - frame Injection
 keyloading(frame,registers, 5, state, deg, 22);

 //Step 4 - 100 IRREGULAR CLOCKS WITH NO OUTPUT
 int i;
 for(i=0; i<100; i++)
 { 
  maj_shift(registers, 5, state, deg, taps);
 }
 
  for(i=0; i<length/8; i++)
 {
  maj5(registers, state, taps, deg, &cipher[i], 8);
  cipher[i] ^= msg[i];
 }
 if(length%8>0){
  maj5(registers, state, taps, deg, &cipher[i], length%8);
  cipher[i] ^= msg[i];   
  }
}
void maj5_decrypt(uint8_t* msg, uint64_t key, uint8_t* cipher, int length)
{
  maj5_encrypt(msg, key, cipher, length);
}

