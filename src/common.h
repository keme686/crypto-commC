#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#include <openssl/bio.h>
#include <openssl/bn.h>
#include "bunny24.h"
#include "StreamCipher.h"
#include "sponge.h"
#include "rsa.h"
#include "prng.h"

#define MSG_SIZE_MAX		2048
#define CONNECTION_STRING	"Hello!"
#define CLOSE_CONNECTION_STRING	"Bye"
#define OK_STRING		"OK"

#define ACK_MSG "MESSAGE RECEIVED AND DECRYPTED!"
#define CORRUPTED_MSG "CORRUPTED MESSAGE RECEIVED!"

#define ENCRYPTED_MSG_MAX 128

void print_buff(u_int8_t *, size_t);
int open_channel(const char *);
void close_channel(int);
ssize_t read_msg(int, u_int8_t **);
ssize_t write_msg(int, const u_int8_t *, u_int16_t);

int read_string(int, const char *);
int write_OK(int);
int write_BYE(int);

void cipher_suite_table(char suite_id, uint8_t *symm_cipher,
                        uint8_t *hash, uint8_t* asymm_cipher);

void printBinary(uint64_t x, int d);
void decrypt_m(uint8_t cipher_id, uint8_t *c, int c_len, uint8_t *k);
void enc_msg(uint8_t cipher_id, uint8_t* c, unsigned int length, uint8_t* k);

int readRsaKey(char* filename, BIGNUM* n, BIGNUM* e);
int readClientsPublicRsaKey(char* filename, char* clientname, BIGNUM* n, BIGNUM* e);

#endif
