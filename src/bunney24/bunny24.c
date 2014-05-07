#include "../bunny24.h"

#include<stdio.h>
void printBv(uint64_t x, int d)
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
 printf("\n");
   return;
}
/*
  change 24 bit message to 4 words with 6 bits
*/
void changeRep(uint64_t msg, uint8_t* output)
{
  int i;
  for(i=0; i<24; i++)
  {
    int bit;    
      bit = (msg>>23)&1?1:0;  //get the left most bit
      output[i/6]<<=1;  //shift left
      output[i/6]^=bit; //add to right most bit of the output word
      msg<<=1;      //shift the message to left (forget about what we just write to the output word)
  }
}

uint8_t Sboxn(uint8_t vector, uint8_t n)
{
 int i;
 uint8_t c = vector;
 for(i=0; i<n-1; i++)
 {
   c= field_product(vector, c);
 }
 return c;
}

/*
  Sbox1
*/
uint8_t Sbox1(uint8_t vector)
{
 int i;
 uint8_t c = vector;
 for(i=0; i<61; i++)
 {
   c= field_product(vector, c);
 }
 return c;
}

/*
  Sbox2
*/
uint8_t Sbox2(uint8_t vector)
{
 int i;
 uint8_t c = vector;
 for(i=0; i<4; i++)
 {
  c= field_product(vector, c);
 }
 return c;
}
/*
  Sbox3
*/
uint8_t Sbox3(uint8_t vector)
{
 int i;
 uint8_t c = vector;
 for(i=0; i<16; i++)
 {
   c= field_product(vector, c); 
 }
 return c;
}

/*
  Sbox4
*/
uint8_t Sbox4(uint8_t vector)
{
 uint8_t c = vector;
 //x^62
 c = Sbox1(vector);

 uint8_t e =2;
 //e^2
 e = field_product(e, e);
 //x^62 + e^2
 return c^e;
}

/*
  SBox
*/
void Sbox(uint8_t* msg)
{
 msg[0] = Sbox1(msg[0]);
 msg[1] = Sbox2(msg[1]);
 msg[2] = Sbox3(msg[2]);
 msg[3] = Sbox4(msg[3]);
}

void Sbox_inv(uint8_t* msg)
{
 msg[0] = Sboxn(msg[0], 62);
 msg[1] = Sboxn(msg[1], 38);
 msg[2] = Sboxn(msg[2], 26);

uint8_t e =2;
 //e^2
 e = field_product(e, e);

 msg[3] = Sboxn(msg[3] ^ e, 62);
}
/*
  Key Scheduler
*/
void keyschedule(uint8_t* keys, uint8_t output[][4])
{
  uint8_t w[88];

  w[0] = keys[0];
  w[1] = keys[1];
  w[2] = keys[2];
  w[3] = keys[3];
  
  w[4] = field_add(Sbox1(w[0]), w[1]);
  w[5] = field_add(Sbox2(w[1]), w[2]);
  w[6] = field_add(Sbox3(w[2]), w[3]);

  w[7] = field_add(Sbox4(w[3]),w[0]);

  int i; 
  for(i=8; i<88; i++)
   {
    if(i%4 != 0)
      w[i] = field_add(w[i-8], w[i-1]);
    else if(i%8==0)
     {   
      w[i] = field_add(field_add(w[i-8], Sbox2(field_rotate(w[i-1],5))), 0x2A);
      }
   else if(i%8 == 4)
      w[i] = field_add(w[i-8],Sbox3(w[i-1]));
   }

 uint8_t boxes[4][5][4];

 int a=0, b = 0,c =0;
 for(i=8; i<88; i++)
  {
   boxes[c][b][a] = w[i];      
   a++;
   if(a==4){
     b++;
     a=0;
     if(b==5){
       b=0;
       c++;
     }
    }
  }

  int l=0;
 for(c=0; c<4; c++)
  {
   output[l][0] = boxes[c][0][0];
    output[l][1] = boxes[c][1][1];
    output[l][2] = boxes[c][2][2];
    output[l][3] = boxes[c][3][3];
    l++;
    if(l==15)
      break;
    output[l][0] = boxes[c][0][1];
    output[l][1] = boxes[c][1][2];
    output[l][2] = boxes[c][2][3];
    output[l][3] = boxes[c][4][0];
      l++;
    output[l][0] = boxes[c][0][2];
    output[l][1] = boxes[c][1][3];
    output[l][2] = boxes[c][3][0];
    output[l][3] = boxes[c][4][1];
    l++;
    output[l][0] = boxes[c][0][3];
    output[l][1] = boxes[c][2][0];
    output[l][2] = boxes[c][3][1];
    output[l][3] = boxes[c][4][2];
    l++;
    output[l][0] = boxes[c][1][0];
    output[l][1] = boxes[c][2][1];
    output[l][2] = boxes[c][3][2];
    output[l][3] = boxes[c][4][3];
    l++;
 }
}
//concatenate 6 bit words to one 64 bit value
uint64_t concat_24(uint8_t* values, uint8_t n)
{
 int i;
 uint64_t result=0x0;
 for(i=0; i<n; i++)
 { 
  result<<=6; 
  result ^= ((uint64_t)values[i]);  
 }
  return result;
}

//encrypts the 24-bit msg by 24 bit key
void encrypt(uint8_t* msg, uint8_t* key)
{
 int i;
 for(i=0; i<4; i++)
   msg[i] = field_add(msg[i], key[i]);
// return 0;
}

/*
  mixing layer
*/
void mixing_layer(uint8_t* msg, int op)
{
 uint8_t lambda[4][4]={ {0x23,0x3B,0x38,0x3D},{0xD,0x3C,0x16,0x18},{0x3,0x20,0x17,0x37},{0x2C,0x26,0x38,0x13} };
 uint8_t lambda_dec[4][4] = {{0x1d, 0x3, 0xb, 0x19},{0x11, 0x2f, 0x3e, 0x3d},{0x7, 0x17, 0x39, 0xc},{0xa, 0x3a, 0xd, 0x29}};
 int i, j;
 uint8_t temp[4]={0};
 //do matrix multiplication and add them up
if(op ==0){
 for(i=0; i<4; i++)
 {  
   for(j=0; j<4; j++)
   {
    temp[i] = field_add(temp[i], field_product(msg[j],lambda[j][i]));
   }
 }
 }else{
  for(i=0; i<4; i++)
 {  
   for(j=0; j<4; j++)
   {
    temp[i] = field_add(temp[i], field_product(msg[j],lambda_dec[j][i]));
   }
 }
 }
 //copy temp to msg
 for(i=0; i<4; i++)
 {
  msg[i] = temp[i];
 }
}

/*
 * Round Function
*/
void RoundFunc(uint8_t* msg, uint8_t* key)
{
 Sbox(msg);
 mixing_layer(msg,0);  //mixing layer with operation 0 = encryption
 encrypt(msg, key);
}

/*
 * this is the public method to encrypt a single 24 bit message with 24 bit key
*/
void BunnyTn_enc(uint64_t msg, uint64_t key,uint64_t* cipher)
{
  uint8_t msg_blocks[4]={0};
  uint8_t keys[16][4];
  uint8_t key_blocks[4]={0};
  changeRep(key, key_blocks);  //change the key into 4 blocks of word
  changeRep(msg, msg_blocks);  //change the msg into 4 blocks of word

  keyschedule(key_blocks, keys);  //key scheduling
 int j;
  encrypt(msg_blocks, keys[0]);   //whitening operation
  
 // printf("key 0: ");
   for(j=0; j<4; j++);

   
//output  
  int i;
  for(i=1; i<16; i++)
  {
   RoundFunc(msg_blocks, keys[i]);    //round function   
  } 

  uint64_t ipher = concat_24(msg_blocks, 4); 
  *cipher = ipher;
}
//len = length of result
void byte2long(uint8_t* bytes,const unsigned int len,uint8_t nbits,  uint64_t* result)
{
  uint8_t i;  
  for(i=0;i<(uint8_t)len; i++)
  {
   
    *result <<=8;
    *result ^=bytes[i];
  }  
}
void byte2longvector(uint8_t* bytes,const unsigned int len,uint8_t nbits,  uint64_t* result)
{
  uint8_t i;  
 // printf("nbits: %d\n", nbits);
  for(i=0;i<(uint8_t)len; i++)
  {
    result[i/3] <<=8;
    result[i/3] ^=bytes[i];
  }  
}

void long2byte(uint64_t* lmsg, unsigned int n, uint8_t* bytes)
{
 uint64_t c=0;
 int i;
 for(i=0; i<n; i++)
 {
  c = 0xFF0000;
  c = lmsg[i/3]&c;

  bytes[i] = c>>16;

  lmsg[i/3]<<=8;
 }
}
//n = length of an msgs array
void BunnyTn_encrypt(uint8_t* msgs, unsigned int n, uint64_t key, uint8_t* cipher)
{
 if(n==0) return;
  int length = 1;
  length = n/3 + (n%3>0?1:0);
 if(!length)
   return;

 uint64_t messages[length];
 uint64_t ciphers[length];
 int i;
 byte2long(msgs, length, n, messages);
 for(i=0; i<length; i++)
 {
    uint64_t ci=0;
    BunnyTn_enc(messages[i], key, &ci);    
    ciphers[i] = ci;
 }
 long2byte(ciphers, n, cipher);
}

/*
 * This is a public function to decrypt a single 24 bit message by 24 bit key
*/
void BunnyTn_dec(uint64_t msg, uint64_t key,uint64_t* plain)
{
  uint8_t msg_blocks[4];
  uint8_t keys[16][4];
  uint8_t key_blocks[4]={0};

  changeRep(key, key_blocks);  //change the key into 4 blocks of word
  changeRep(msg, msg_blocks);  //change the msg into 4 blocks of word

  keyschedule(key_blocks, keys);  //key scheduling

   int j;
   for(j=0; j<16;j++);
   int i;
   for(i=15; i>0; i--)
   {
    encrypt(msg_blocks, keys[i]);
    mixing_layer(msg_blocks,1);  //mixing layer with operation 1 = decryption/inverse
    Sbox_inv(msg_blocks);
   }
   encrypt(msg_blocks, keys[0]);
  uint64_t lain = concat_24(msg_blocks, 4); 
  *plain = lain;  
}
void BunnyTn_decrypt(uint8_t* cipher, unsigned int n, uint64_t key, uint8_t* plain)
{
  if(n==0) return;
  int length = 1;
  length = n/3 + (n%3>0?1:0);
 if(!length)
   return;

 uint64_t ciphers[length];
 uint64_t messages[length];
 int i;
 byte2long(cipher, length, n, ciphers);
 for(i=0; i<length; i++)
 {
    uint64_t ci=0;
    BunnyTn_dec(ciphers[i], key, &ci);    
    messages[i] = ci;
 }
 long2byte(messages, n, plain);
}
/*
 * BunnyTnCBC() - Cipher-block chaining (CBC) using bunny
*/
void BunnyTncbc_enc(uint64_t* msg, uint8_t n, uint64_t key, uint64_t iv, uint64_t* out)
{
   uint8_t i;     
   uint64_t tm = msg[0]^iv;   
   BunnyTn_enc(tm, key, &out[0]);
   for(i=1; i<n; i++){
    tm = msg[i] ^ out[i-1];
    BunnyTn_enc(tm, key, &out[i]);
   }
 }
void fromByte(uint8_t* msg, int msglen, uint64_t* result)
{
  int i; 
  for(i=0; i<msglen; i++)
  {
   if(i%3 ==0)
     result[i/3] = 0; 
   result[i/3] <<=8;
   result[i/3] ^=msg[i];
   
  }
}

/*
 *  bunny CBC encrypt public interface
 *
 *
 */
//n = length of an msgs array
void  BunnyTncbc_encrypt(uint8_t* msgs, uint8_t n, uint8_t* k, uint64_t iv, uint8_t* cipher)
{
 int i;
 if(n==0) return;
  int length = 1;
  
  length = n/3 + (n%3>0?1:0);
  uint64_t key[1] ={0};
  fromByte(k, 3, key);
  uint64_t messages[length], ciphers[length];
 fromByte(msgs, n, messages);
 BunnyTncbc_enc(messages,length, key[0], iv, ciphers);   
 
 long2byte(ciphers, n, cipher); 
  
 return;
}

void BunnyTncbc_decr(uint64_t* cipher, unsigned int n, uint64_t key, uint64_t iv, uint64_t* output)
{
   int i;      
   BunnyTn_dec(cipher[0], key, &output[0]);
   output[0] ^= iv;       
   for(i=1; i<n; i++){   
    BunnyTn_dec(cipher[i], key, &output[i]);    
    output[i] ^=cipher[i-1];
   }
}
/*
 *  bunny CBC decrypt public interface
 *
 *
 */
//n = length of an msgs array
void Bunnycbc_decrypt(uint8_t* msgs,uint8_t n, uint8_t* k, uint64_t iv, uint8_t* cipher)
{
 if(n==0) return;
  int length = 1;
  length = n/3 + (n%3>0?1:0);
  uint64_t key[3]={0};
   fromByte(k, 3, key);
  uint64_t messages[length], ciphers[length];
 fromByte(msgs, n, messages);
 BunnyTncbc_decr(messages,length, key[0], iv, ciphers);   
 long2byte(ciphers, n, cipher);  
}


