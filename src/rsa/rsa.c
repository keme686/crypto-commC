#include "../rsa.h"

void encrypt_msg(BIGNUM* m, const BIGNUM* e, const BIGNUM* n) {
  BIGNUM* tmp;
  BN_CTX *ctx;
  ctx = BN_CTX_new();
  tmp = BN_new();
  BN_copy(tmp, m);  /* tmp = m */

  BN_mod_exp(m, tmp, e, n, ctx); /* m = tmp ^ e (mod n) */
   
  BN_free(tmp);
  BN_CTX_free(ctx);
}

void decrypt_msg(BIGNUM* m, const BIGNUM* e, const BIGNUM* n) {
  encrypt_msg(m, e, n);
}

/* encrypts (or decrypts) with private key, not sensitive to
   timing attacks (blind encryption)
*/
void encrypt_msg_secure(BIGNUM* m, const BIGNUM* d,
                        const BIGNUM* e, const BIGNUM* n,
                        const unsigned char * r_bin, int r_len) {
  BN_CTX *ctx;
  BIGNUM *tmp = BN_new();
  BIGNUM *r = BN_new();
  BIGNUM *r_inv = BN_new();

  ctx = BN_CTX_new();
  BN_bin2bn(r_bin, r_len, r);
  BN_mod(r, r, n, ctx); /* r = r % n */ 
  BN_mod(tmp, n, r, ctx);
  while (BN_is_zero(tmp)) { /*  */
    BN_mod_add(r, r, BN_value_one(), n, ctx);
    BN_mod(tmp, n, r, ctx);
  }

  BN_mod_inverse(r_inv, r, n, ctx);
  BN_mod_exp(r, r, e, n, ctx);  /* r = r^e % n */
  BN_mod_mul(m, m, r, n, ctx);  /* m = m * r % n */

  encrypt_msg(m, d, n);

  BN_mod_mul(m, m, r_inv, n, ctx);

  BN_free(r);
  BN_free(r_inv);
  BN_free(tmp);
  BN_CTX_free(ctx);
}
