#ifndef _STREAMCHIPER_H
#define _STREAMCHIPER_H
#include<stdint.h>
#include "Maj5.h"
#include "All5.h"
#include "A5_1.h"

typedef enum { MAJ5, ALL5, A51 } STREAMType;

void stream_encrypt(STREAMType type, uint8_t *plain, uint8_t *chiper, int length);
void stream_decrypt(STREAMType type, uint8_t *chiper, int length);


#endif
