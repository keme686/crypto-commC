#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#include "All5.h"
uint8_t msg[70]={0x12,0xB5,0x76,0x2E,0xE2,0x60,0x30,0x54,0xDA,0x64,0x19,0x30,
              0x4B,0x7E,0xE0,0x22,0x37,0x4D,0xDA,0x6D,0xD4,0x65,0x72,0xC7 };

uint64_t registers[5]={0x7,0xD, 0x19,0xC1,0x1C3}; 

int deg[5]={2, 3, 4, 7, 8}; 

uint64_t poly = 0x1C3;

void printBinary(uint64_t x, int d)
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

void all(int i, int j, int k, int l, int s, uint8_t* cipher, int length)
{
 uint64_t state[5]={i, j, k, l, s};
 
 for(i=0; i<length/8; i++)
 {
  cipher[i]=0; 
  all5(registers, state, deg, &cipher[i], 8);
 }

 if(length%8>0){
   cipher[i]=0;
   all5(registers, state, deg, &cipher[i], length%8);
  }
}

void decreptall(uint8_t* stream)
{
 
  int i;
  printf("cipher found->\n");
  for(i=0; i<70; i++)
  {
   printBinary(msg[i], 8);
  }
  printf("\nstream found after attack->\n");
  for(i=0; i<70; i++)
  {
   printBinary(stream[i], 8);
  }
  printf("\n----------------------------------------------\nMessage decrepted:");

  for(i=33; i<70; i++)
  {
   uint8_t ch = msg[i-33]^stream[i];
   printf("%c", (char)ch);
  }
  printf("\n----------------------------------------------\n");
}
void generateKeyStream()
{
  uint8_t s[100];
  uint64_t state[5]={2, 5, 7, 57, 177};
  uint8_t bits[5]={0};
  uint8_t out[32] = {0};

  int i, j, bit=0;
  for(j=0; j<32*8; j++)
  {
   for(i=0; i<5; i++) 
   {
   bits[i] = (state[i] & 1)?1:0;    
   const int l=1;   
   lfsr(&registers[i],&state[i],deg[i],l,s); 
   }  
  bit= ((bits[0] & bits[1] & bits[2] & bits[3] & bits[4]) ^ (bits[3] & bits[4]) ^ bits[4]);
  printf("%d,", bit); 
   
  }
printf("\n");
}


int main()
{  
  //generateKeyStream();

int st =  0;
  int i, count=0;

  uint8_t output[200]={0};
  uint8_t stream[200]={0x90,0x44,0x81,0x02,0x48,0x68,0xD0,0x1D,0x80,0xDA,0x88,0x29,0x20,0x12,0x1C,0x10,0x26,0x15,0x83,0x19,0x0C,
0xE9,0x90,0x25,0xC8,0x3B,0x21,0x04,0x0B,0x2C,0x9C,0x49};
     /*
    {0x28,0x6C,0x04,0x08,0xC4,0x46,0x00,0x90,0x53,0x01,0x48,0x4A,0x5A,0x02,0x02,0x01,
                      0x2A,0x99,0x60,0x06,0x69,0x80,0x41,0x7A,0xAC,0x22,0x10,0xD1,0x24,0xD4,0x62,0x90};
*/
   for(i=1; i<=255; i++)
    {
     uint8_t lfs[32]={0}; 
      uint64_t state =i;
     lfsr(&poly, &state, 8, 32*8, lfs);
    int x= 0, k, y;
    for(y=0; y<32; y++){  
     for(k=7; k>=0; k--)
      {
       int bit = (lfs[y] >>k)&1; 
       int cbit = (stream[y] >>k)&1;
       if(bit == cbit){
           x++;           
          }
         }   
      } 
     if(count < x)
     {
      count =x; 
       st = i;
     }  
    } 

    printf("Found initial state: ");
    printBinary(st, 8);
    printf("\nCount: %d\n", count);

    int j,k,l;
    int s1=0, s2=0, s3=0, s4=0;
    int found = -1;
     int t, u; 
    int bit=0, cbit=0;
    for(i=0; i<=3; i++)
    {     
     for(j=0; j<=7; j++)
      {       
        for(k=0; k<=15; k++)
         {          
        for(l=0; l<=127; l++)
          {                
               all(i, j, k, l, st, output, 32*8);                
                for(t=0; t<32; t++){
		 for(u=7; u>=0; u--)
                    {
                     bit = (output[t] >>u)&1; 
                     cbit = (stream[t] >>u)&1;                  
                     if(bit != cbit){
                      found = -1;       
                      goto cont;
                       }
                     }
                 } 
		found = 1;
                s1=i; s2=j; s3=k; s4=l;
                goto next;
           cont:
             found=-1;   
          }
         }
      }
    }
    next:
       if(found==1)
        {
          printf("found: %d %d %d %d %d\n", s1, s2, s3, s4, st);
          decreptall(stream);
         }
        else{
           printf("not found\n");
          }


 return 0;
}
