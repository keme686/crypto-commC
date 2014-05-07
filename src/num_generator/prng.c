#include "../prng.h"
#include "../Maj5.h"

//len = length of result
void byte2longr(uint8_t* bytes, unsigned int len,unsigned int nbits,  uint64_t* result)
{
  int i;
  *result = 0;
  for(i=0;i<nbits; i++)
  {
    *result <<=8;
    *result ^=bytes[i];
  }  
}

void getRandom(uint8_t* num, int length)
{
  FILE *fp;
  int i;
  fp = fopen("/dev/random", "r");
    for (i=0; i<length; i++) {
      fscanf(fp, "%c", &(num[i]));
    }   
    fclose(fp);
}

void fprng(uint8_t* num, int length)
{ 
  unsigned int iseed = (unsigned int)time(NULL);
  srand (iseed);
  int i;
  uint8_t msg[8];
  uint64_t state = 0;
  for(i=0; i<19; i++)
  {
   int j = (int)(2.0*rand() /(RAND_MAX + 1.0));
   state<<=1;
   state ^=j;
  }
 uint64_t key =   0;
 for(i=0; i<length; i++)
  {
   int j = (int)(2.0*rand() /(RAND_MAX + 1.0));
   key<<=1;
   key ^=j;
  }
 uint64_t poly = 0x80027;
 lfsr(&poly, &state, 19, length, msg); //num->msg

  maj5_encrypt(msg, key, num, length);
 //all5_encrypt(state, key, num, 64);
}

void sprng(uint8_t* num, int length)
{
  FILE *fp;
  uint8_t seed[16];
  int i;
  uint8_t key[3];
  uint8_t iv[3];
  unsigned int n = length/8 + (length%8>0?1:0);
  uint8_t msg[n];

  fp = fopen("/dev/random", "r");
    for (i=0; i<3; i++) {
      fscanf(fp, "%c", &(key[i]));
    }
    for (i=0; i<3; i++) {
      fscanf(fp, "%c", &(iv[i]));
    }
    for (i=0; i<n; i++) {
      fscanf(fp, "%c", &(msg[i]));
    }
    fclose(fp);
  uint64_t k=0;
  uint64_t ivk=0;
  byte2longr(key, 1, 24, &k);
  byte2longr(iv, 1, 24, &ivk);
  BunnyTncbc_encrypt(msg, n, k, ivk, num); 
}

void primeNum(uint8_t* num, int length)
{   
    time_t current_time; 
    /* Obtain current time as seconds elapsed since the Epoch. */
    current_time = time(NULL);

      int is_p ;
	BIGNUM *n;	
	n = BN_new();

	int exit = 0 , i = 0;
	int byteLength = length / 8 + ((length % 8 != 0) ? 1 : 0); //length in byte
        uint64_t numbers = 0;
	do{		
 	    fprng(num , length);	//generate a number	         
            num[byteLength-1] |= 0x01;  /* Last bit as 1 */	
	    numbers++;
	    //from array of bits to array of byte --> i / 8 : current byte m, i % 8 : current bit inside the byte	
	    n = BN_bin2bn((const unsigned char *) num, byteLength , NULL);	//to big number		
	    
            is_p = BN_is_prime(n,10,NULL, NULL, NULL) ;
            if (is_p == -1)
                 printf("ERROR!\n") ;
            if (is_p ==  1){
                 printf("n = %s is prime!\n", BN_bn2dec(n)) ;
		exit=1;
	      }

	}while(exit == 0);

    printf("time: ");
    time_t fcurrent_time; 
    /* Obtain current time as seconds elapsed since the Epoch. */
    fcurrent_time = time(NULL);
 
    if (fcurrent_time == ((time_t)-1))
    {
        (void) fprintf(stderr, "Failure to compute the current time.");
        return;;
    }
     printf("current time: %lld \n", (long long)(fcurrent_time-current_time));
}
