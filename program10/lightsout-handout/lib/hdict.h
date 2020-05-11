/*
 * Hash dicts interface
 *
 * 15-122 Principles of Imperative Computation */

#include <stdlib.h>
#include <stdbool.h>

#ifndef _HDICT_H_
#define _HDICT_H_

typedef void *hdict_key;
typedef void *hdict_entry;
typedef hdict_key entry_key_fn(hdict_entry e);
typedef bool key_equal_fn(hdict_key k1, hdict_key k2);
typedef size_t key_hash_fn(hdict_key k);
typedef void key_print_fn(hdict_key k);
typedef void entry_print_fn(hdict_entry e);
typedef void entry_free_fn(hdict_entry e);

typedef struct hdict_header* hdict_t;

hdict_t hdict_new(size_t capacity,
                  entry_key_fn *entry_key,
                  key_equal_fn *key_equal,
                  key_hash_fn *key_hash,
                  entry_free_fn *entry_free)
  /*@requires capacity > 0; @*/
  /*@requires entry_key != NULL && key_equal != NULL && key_hash != NULL; @*/
  /* if entry_free is NULL, then elements will not be freed */
  /*@ensures \result != NULL; @*/ ;

/* If insertion removes an existing element from the dict, that
 * removed element will be returned so that the client can free it, if
 * necessary */
hdict_entry hdict_insert(hdict_t H, hdict_entry e)
  /*@requires H != NULL && e != NULL; @*/ ;

hdict_entry hdict_lookup(hdict_t H, hdict_key k)
  /*@requires H != NULL && k != NULL; @*/ ;

void hdict_print(hdict_t H,
                 key_print_fn* print_key, entry_print_fn* print_entry)
/*@requires H != NULL && print_key != NULL && print_entry != NULL; @*/ ;

void hdict_free(hdict_t H)
  /*@requires H != NULL; @*/ ;

#endif
