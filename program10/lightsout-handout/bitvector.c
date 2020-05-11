#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "lib/xalloc.h"
#include "lib/contracts.h"
#include "lib/bitvector.h"


#if (BITVECTOR_LIMIT <= 8)
#define BITS uint8_t

#elif (BITVECTOR_LIMIT <= 16)
#define BITS uint16_t

#elif (BITVECTOR_LIMIT <= 32)
#define BITS uint32_t

#else
#define BITS uint64_t

#endif

bitvector bitvector_new()
{
  BITS res = 0;
  return res;
}

bool bitvector_get(bitvector B, uint8_t i)
  /*@requires 0 <= i < BITVECTOR_LIMIT; @*/
{
  REQUIRES(i < BITVECTOR_LIMIT);
  BITS index = (BITS) i;
  return ((B & (1 << (index - 1))) != 0);
}

bool bitvector_equal(bitvector B1, bitvector B2)
{
  BITS b1 = (BITS) B1;
  BITS b2 = (BITS) B2;
  return (b1 == b2);
}

bitvector bitvector_flip(bitvector B, uint8_t i)
  /*@requires 0 <= i < BITVECTOR_LIMIT; @*/ 
{
  REQUIRES(i < BITVECTOR_LIMIT);
  BITS index = (BITS) i;
  BITS res = (B ^ (1 << (index - 1)));
  return res;
}
