#include "../sponge.h"

//n = number of bytes
void padding(uint8_t* m, unsigned int n, uint64_t* out)
{
  int i=0, k=0;
   while(i<n)
   {    
    out[k] = 0;
    out[k] ^= (((uint64_t)m[i])<<12);
    i++;
    if(i>=n) break;
    out[k] ^= (((uint64_t)m[i])<<4);
    i++;
    if(i>=n) break;
    out[k] ^= (m[i]>>4);
    uint8_t v = m[i] <<4;
   
    i++;
    k++;
    out[k] = 0;
    if(i>=n) break;
    out[k] ^= (((uint64_t)v)<<12);    
    out[k] ^=  (((uint64_t)m[i])<<8);     
    i++;    
   if(i>=n) break;   
    out[k] ^=  (((uint64_t)m[i]));    
    i++;    
    k++;
    if(i>=n) break;
   }
}
//n = number of blocks
void absorption(uint64_t* state, unsigned int n, uint64_t* out, uint64_t key)
{
   //absorption phase
 int i;
 *state = 0;
 for(i=0; i<n; i++)
 {  
  *state ^=(out[i]<<4);  
  uint64_t stx = 0;
  BunnyTn_enc(*state, key, &stx); 
  *state = stx; 
 }
}

void squeezingByte(uint64_t* state, uint8_t* out, uint64_t key)
{
  //squeezing
// uint8_t outp[20] ={0};  //20*8 = 160 bit output stream
 int i, k=0;
 uint64_t temp=0;
 for(i=0; i<4; i++)
 {
  //first 20 bit  
  temp = *state & 0xFFFFF0;
  temp>>=4;

  out[k] =(uint8_t) (temp>>12);  
  k++;
  out[k] =(uint8_t) ((temp>>4)&0x000FF);  
  k++;
  out[k] =(uint8_t) ((temp&0x00000F)<<4); 
   
  BunnyTn_enc(*state, key, state); 
  //second 20 bit
  temp = *state & 0xFFFFF0;
  temp>>=4;
  out[k] ^=(uint8_t) (temp>>16);  
  k++;
  out[k] =(uint8_t) ((temp>>8)& (uint64_t)0x0000FF);   
  k++;
  out[k] =(uint8_t) (temp&0x0000FF);  
  k++;
  BunnyTn_enc(*state, key, state);
 } 
 
}
//n = number of bytes of msg
void sponge_hash(uint8_t* msg,unsigned int n,  uint8_t* output)
{
  uint64_t key = 0xFFFFFF;
  int i, bit = n*8;
  int a = bit/20;
  a+=(bit%20)>0?1:0;
  uint64_t out[a];
  padding(msg, n, out);

  uint64_t state = 0;
  uint8_t outbyte[20] = {0};
  absorption(&state, a, out, key);
  squeezingByte(&state, outbyte, key);
 for(i=0;i<20; i++){
  output[i]= 0;
  output[i] = outbyte[i];
  }
}
