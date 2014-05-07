#include "../lfsr.h"
#include<stdio.h>

void lfsr(uint64_t* poly, uint64_t* state, int deg, const int out_len, uint8_t output[])
{  
  int i=0, j=0;  
  for(i=0; i<out_len; i++)
  {
   int out = *state&1;
   //output[i] = out?'1':'0';  //for A5/1    
   int bit=0;
    int a = out?1:0;
    output[i/8] <<=1;
    output[i/8] ^=a;  
   uint64_t temp=*poly;
   for(j=0; j<deg;j++)
    {     
     int k=temp&1;
      if(k==1){	 
         bit^= (*state>>j)& 1;
       }
      temp>>=1;     
     }   
  //   output[i] = bit?'1':'0';  //lfsr test vector
    
   *state>>=1;
   *state ^=(bit<<(deg-1));
  }   
}

