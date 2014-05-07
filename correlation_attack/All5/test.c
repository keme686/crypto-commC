#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#include "../All5.h"


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

int test_All(uint64_t key, uint64_t state, uint8_t* stream)
{
  uint8_t lfs[32]={0};
 uint64_t poly = 0x1C3;
 int i,k, count=0;
uint8_t msg[70]="while wrong initial states generate sequences with different behaviour";
 uint8_t cipher[70]={0};
 all5_encrypt(msg, key, cipher, 70*8);  
 for(i=0; i<32; i++)
 {  
  stream[i]= cipher[i] ^ msg[i];
 }
uint64_t st = state;
 lfsr(&poly, &state, 8, 32*8, lfs);
  
 for(i=0; i<32; i++){
   for(k=7; k>=0; k--)
      {
       int bit = (lfs[i] >>k)&1; 
       int cbit = (stream[i] >>k)&1;
        if(bit == cbit){
           count++;           
          }
      }   
  } 
 return count;
}
void all(int i, int j, int k, int l, int s, uint8_t* cipher, int length)
{
 uint64_t registers[5]={0x7,0xD, 0x19,0xC1,0x1C3};  
 uint64_t state[5]={i, j, k, l, s};
 int deg[5]={2, 3, 4, 7, 8}; 
 
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
void decreptall(int i, int j, int k, int l, int s, int length, uint64_t key)
{
 uint64_t registers[5]={0x7,0xD, 0x19,0xC1,0x1C3};  
 uint64_t state[5]={i, j, k, l, s};
 int deg[5]={2, 3, 4, 7, 8}; 
 uint8_t msg[70]="while wrong initial states generate sequences with different behaviour";
 uint8_t cipher[70]={0};
 all5_encrypt(msg, key, cipher, 70*8); 

 uint8_t stream[70]={0};
 for(i=0; i<length/8; i++)
 {
  all5(registers, state, deg, &stream[i], 8);
 }
 if(length%8>0){
  all5(registers, state, deg, &stream[i], length%8);
 }
  printf("cipher found->\n");
  for(i=0; i<70; i++)
  {
   printBinary(cipher[i], 8);
  }
  printf("\nstream found after attack->\n");
  for(i=0; i<70; i++)
  {
   printBinary(stream[i], 8);
  }
  printf("\n----------------------------------------------\nMessage decrepted:");

  for(i=0; i<70; i++)
  {
   uint8_t ch = cipher[i]^stream[i];
   printf("%c", (char)ch);
  }
  printf("\n----------------------------------------------\n");
}


int main()
{
 uint64_t key =   0x48C4A2E691D5B3F7;
  unsigned int iseed = (unsigned int)time(NULL);
  srand (iseed);
  int st =  0;
   int i, count=0;
  uint8_t output[70]={0};
  uint8_t stream[70]={0};
   for(i=1; i<=255; i++)
    {
    int x= test_All(key, i, stream);
     if(count < x)
     {
      count =x; 
       st = i;
     }  
    } 
    printf("Found initial state: ");
    printBinary(st, 8);
    printf("\nCount: %d\n", count);

    int j,k,l,m;
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
          decreptall(s1, s2, s3, s4, st, 70*8, key);
         }
        else{
           printf("not found\n");
          }
 return 0;
}
