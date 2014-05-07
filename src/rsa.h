#ifndef _RSA_H_
#define _RSA_H_

#define N_PRIMES 2049

#include <openssl/bn.h>
#include "prng.h"

extern const int primes[N_PRIMES];

void generate_random_prime(BIGNUM *p, int key_len);

void encrypt_msg(BIGNUM* m, const BIGNUM* e, const BIGNUM* n);
void decrypt_msg(BIGNUM* m, const BIGNUM* d, const BIGNUM* n);
void encrypt_msg_secure(BIGNUM* m, const BIGNUM* d,
                        const BIGNUM* e, const BIGNUM* n,
                        const unsigned char * r_bin, int r_len);

#endif
