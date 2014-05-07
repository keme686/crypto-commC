#include "rsa.h"
#include<stdlib.h>
#include<stdint.h>

int main(int argc, char **argv) {
  BIGNUM *p, *q, *pmq, *n, *e, *d, *psi, *pm1, *qm1;
  int key_len;

  BN_CTX *ctx = BN_CTX_new();

  p = BN_new();
  q = BN_new();
  pmq =  BN_new();

  n = BN_new();
  e = BN_new();
  d = BN_new();
  psi = BN_new();
  pm1 = BN_new();
  qm1 = BN_new();

  if (argc != 2) {
    printf("./keygen [keylen]\n");
    return 1;
  }

  key_len = atoi(argv[1]) / 8;
  int length = atoi(argv[1]);
  
  printf("Key length: %d bits\n", length);
   uint8_t num[8], num2[8];
   primeNum(num, length);
   p = BN_bin2bn((const unsigned char *) num,length/8 , NULL);
   printf("p = %s !\n", BN_bn2dec(p)) ;
   printf("\nGenerated p...\n");

   primeNum(num2, length);
   q= BN_bin2bn((const unsigned char *) num2, length/8 , NULL);
   printf("q = %s !\n", BN_bn2dec(q)) ;
   printf("\n Generated q...\n");

  BN_mul(n, p, q, ctx);

  BN_sub(pm1, p, BN_value_one());
  BN_sub(qm1, q, BN_value_one());

  BN_mul(psi, pm1, qm1, ctx);

  BN_zero(e);
  BN_add_word(e, (BN_ULONG) (65539));

  BN_mod_inverse(d, e, psi, ctx);

  printf("N: ");
  BN_print_fp(stdout, n);
  printf("\n");

  printf("P: ");
  BN_print_fp(stdout, p);
  printf("\n");

  printf("Q: ");
  BN_print_fp(stdout, q);
  printf("\n");

  printf("E: ");
  BN_print_fp(stdout, e);
  printf("\n");

  printf("D: ");
  BN_print_fp(stdout, d);
  printf("\n");

  BN_free(p);
  BN_free(q);
  BN_free(n);
  BN_free(e);
  BN_free(d);
  BN_free(psi);
  BN_free(pm1);
  BN_free(qm1);
  BN_free(pmq);

  BN_CTX_free(ctx);

  return 0;
}
