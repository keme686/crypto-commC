#ifndef _LFSR_H_
#define _LFSR_H_

#include <stdint.h>
#include <stdlib.h>

/*  a linear feedback shift register:
 * poly   = poynomial in the vector form, indicating tap positions
          (if poly = x^3 + x^2 + x^0 then we have the following LFSR: )
          (                                                        )   
          (    x^0  x^1__x^2__x^3_                                 )
          (    /-->|____|____|____|                                )
          (    |           |    |                                  )
          (    \-----------+----/                                  )
          (                                                        )
 * state   = initial state vector
 * deg     = degree of the polynomial
 * out_len = output stream length, i.e. number of bits to output
 * output  = output of the LFSR function
*/
void lfsr(uint64_t* poly, uint64_t* state, int deg,const int out_len, uint8_t* output);

#endif
