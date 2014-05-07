#include "../A5_1.h"

/*
 *  key/frame Loadding   - overwrites on state values
 *
 * key       = 64-bit key
 * registers = array of registers
 * num_reg   = number of registers
 * state     = array of state vectors
 * deg       = array of integer indicating degree of the registers
 * out_len   = number of output bits
*/
void keyloading(uint64_t key,uint64_t* registers, int num_reg, uint64_t* state, int* deg, int out_len)
{
 int i,j;
 uint64_t out;  
 uint8_t s[100];

 for(i=0; i<out_len; i++)
 {
  uint64_t zz = 1;
  zz<<=63;
  out= key&zz;  //get the right most bit
  key <<=1;
  for(j=0;j<num_reg; j++)
  {
   const int l=1;
   lfsr(&registers[j], &state[j], deg[j],l,s);  
   state[j] ^=((out>>63)<<(deg[j]-1));  
  }   
 }
}

/*   shift the majority bit registers - overwrites on state values
 * register   = array of register indicating tap position
 * num_reg    = number of registers
 * state      = array of state vector
 * deg        = array of integer indicating degree of each register
 * tap        = array of clock positions
*/
void maj_shift(uint64_t* registers, int num_reg, uint64_t* state, int* deg, uint64_t* tap)
{  
  
  int i,majority;
  int zero=0, one=0;  
  
  uint8_t s[100];
  
  for(i=0; i<num_reg; i++)  //find majority bit
   {
   if(state[i]&tap[i])  //if tap position of the state is one
       one++;
    else 
       zero++;
   }
  if(one > zero)
    majority=1;
  else
    majority=0;
  zero=0;
  one =0;
  
  for(i=0; i<num_reg; i++)  //shif the state with majority bit
  {
   int tapx = (state[i]&tap[i])?1:0;   
   const int l=1;
   if(tapx==majority){
     lfsr(&registers[i],&state[i],deg[i],l,s);      
     }
  }  
}


/*
 *  A5/1 initialization - initialization will be overwriten on the arrays passed
 *
 * registers = array of registers to be initialized
 * taps      = array of clock positions to be initialized
 * deg       = array of integer to initialize degree of each registers
*/ 
void init_A5_1(uint64_t* registers, uint64_t* state, uint64_t* taps, int* deg)
{
 registers[0]=0x80027;
 registers[1] =0x400003;
 registers[2]= 0x808007; 

 taps[0]=0x400;
 taps[1] =0x800;
 taps[2] =0x1000; //2^9, 2^11, 2^11

 deg[0]=19;
 deg[1]=22;
 deg[2]=23;

 state[0] =0;
 state[1] =0;
 state[2] =0;
}


/*
 *   A5/1
 * key      = 64 bit key
 * output   = output vector
 * out_bits = number of output bits
*/
void a5_1(uint64_t* registers, uint64_t* state, uint64_t* taps, int* deg, uint8_t* output, int out_bits)
{
 int i,j;
 for(i=0; i<out_bits; i++)  //Step 5 - n IRREGULAR CLOCKS WITH OUTPUT
 {  
  int out=0;
  for(j=0; j<3; j++)
  {
   uint64_t temp = state[j] ;
   out ^= temp&((uint64_t)1)?1:0;   
  }
  int a = out?1:0;
  *output ^= a<<(7- (i%8));
  maj_shift(registers, 3, state, deg, taps);    
 } 
}

void a51_encrypt(uint8_t* msg, uint64_t key, uint8_t* cipher, int length)
{
    uint64_t registers[3],state[3],taps[3];
    int deg[3];

 init_A5_1(registers, state, taps, deg);   // Step 1 - INIZIALIZATION

 keyloading(key,registers, 3, state, deg, 64);   //Step 2 - KEY INJECTION

 keyloading(FRAME,registers, 3, state, deg, 22);  // Step 3 - FRAME INJECTION

 int i;
 for(i=0; i<100; i++)    //Step 4 - 100 IRREGULAR CLOCKS WITH NO OUTPUT
 {
  maj_shift(registers, 3, state, deg, taps);
}

 for(i=0; i<length/8; i++)
 {
  a5_1(registers, state, taps, deg, &cipher[i], 8);
  cipher[i] ^= msg[i];
 }
 if(length%8>0){
  a5_1(registers, state, taps, deg, &cipher[i], length%8);
  cipher[i] ^= msg[i];   
  }
 
}

void a51_decrypt(uint8_t* msg, uint64_t key, uint8_t* cipher, int length)
{
  a51_encrypt(msg, key, cipher, length);
}

