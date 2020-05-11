#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "lib/xalloc.h"
#include "lib/contracts.h"
#include "lib/bitvector.h"
#include "lib/hdict.h"
#include "board-ht.h"


void* entry_key(void *D)
{
  struct board_data *DAT = (struct board_data*) D;
  return &DAT->board;
}

bool key_equal(void *a, void *b)
{
  REQUIRES(a != NULL && b != NULL);
  bitvector A = *(bitvector*) a;
  bitvector B = *(bitvector*) b;
  return bitvector_equal(A, B);
}

size_t key_hash(void *k)
{
  REQUIRES(k != NULL);

  bitvector b = *((bitvector*) k);
  size_t res = 0;
  for (int i = 0; i < BITVECTOR_LIMIT; i++)
  {
  	bool tmp = bitvector_get(b, i);
  	if (tmp == true) res++;
  	res = res << 1;
  }
  return res;
}

void entry_free(void *e)
{
  struct board_data *f = (struct board_data*) e;
  free(f);
  return;
}

hdict_t ht_new(size_t capacity)
  /*@requires capacity > 0; @*/
  /*@ensures \result != NULL; @*/ 
{
  REQUIRES(capacity > 0);

  hdict_t H = hdict_new(capacity, &entry_key, &key_equal, &key_hash, &entry_free);

  ENSURES(H != NULL);
  return H;
}

struct board_data *ht_lookup(hdict_t H, bitvector B)
  /*@requires H != NULL; @*/
{
  REQUIRES(H != NULL);
  return hdict_lookup(H, (void*) &B);
}

void ht_insert(hdict_t H, struct board_data *DAT)
  /*@requires H != NULL; @*/ 
{
  REQUIRES(H != NULL && DAT != NULL);
  REQUIRES((ht_lookup(H, DAT->board) == NULL));
  hdict_insert(H, (void*) DAT);
  ENSURES(ht_lookup(H, DAT->board));
  return;
}