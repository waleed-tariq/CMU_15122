/*
 * Hash dicts implementation
 *
 * 15-122 Principles of Imperative Computation */

#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include "xalloc.h"
#include "contracts.h"
#include "hdict.h"

typedef struct chain_node chain;
struct chain_node {
  hdict_entry entry;
  chain* next;
};

typedef struct hdict_header hdict;
struct hdict_header {
  size_t size;
  size_t capacity;               // 0 < capacity
  chain **table;                 // \length(table) == capacity
  entry_key_fn *entry_key;       // entry_key != NULL
  key_equal_fn *key_equal;       // key_equal != NULL
  key_hash_fn *key_hash;         // key_hash  != NULL
  entry_free_fn *entry_free;
};

static inline hdict_key entrykey(hdict* H, hdict_entry e) {
    REQUIRES(H != NULL && H->entry_key != NULL && e != NULL);
    return (*H->entry_key)(e);
}

static inline size_t hashindex(hdict* H, hdict_key k) {
  REQUIRES(H != NULL && H->capacity > 0 && H->key_hash != NULL);
  return (*H->key_hash)(k) % H->capacity;
}

static inline bool keyequal(hdict* H, hdict_key k1, hdict_key k2) {
  REQUIRES(H != NULL && H->capacity > 0 && H->key_equal != NULL);
  return (*H->key_equal)(k1, k2);
}

static bool is_chain(hdict* H, chain* p, size_t i) {
  REQUIRES (H != NULL && H->capacity > 0 && H->key_hash != NULL);
  for (chain* q = p; q != NULL; q = q->next) {
    if (q->entry == NULL) return false;
    hdict_key k = entrykey(H, p->entry);
    if (hashindex(H, k) != i) {
      fprintf(stderr, "Violation: key in index %zu claims ", i);
      fprintf(stderr, "to belong in index %zu.\n", hashindex(H, k));
      fprintf(stderr, "This indicates that client code modifies data ");
      fprintf(stderr, "in the hashtable.\n");
      return false;
    }
  }
  return true;
}

static bool is_chains(hdict* H, size_t lo, size_t hi) {
  REQUIRES(H != NULL && H->capacity > 0 && H->key_hash != NULL);
  REQUIRES(lo <= hi && hi <= H->capacity);
  for (size_t i = lo; i < hi; i++) {
    if (!is_chain(H, H->table[i], i)) return false;
  }
  return true;
}

bool is_hdict(hdict* H) {
  return  H != NULL
      && H->capacity > 0
      && H->entry_key != NULL
      && H->key_equal != NULL
      && H->key_hash  != NULL
      && is_chains(H, 0, H->capacity);
}

hdict* hdict_new(size_t capacity,
                 entry_key_fn *entry_key,
                 key_equal_fn *key_equal,
                 key_hash_fn *key_hash,
                 entry_free_fn *entry_free) {
  REQUIRES(capacity > 0);
  REQUIRES(entry_key != NULL && key_equal != NULL && key_hash != NULL);

  hdict* H = xmalloc(sizeof(hdict));
  H->size = 0;
  H->capacity = capacity;
  H->table = xcalloc(capacity, sizeof(chain*));
  H->entry_key = entry_key;
  H->key_equal = key_equal;
  H->key_hash = key_hash;
  H->entry_free = entry_free;

  ENSURES(is_hdict(H));
  return H;
}

hdict_entry hdict_lookup(hdict* H, hdict_key k) {
  REQUIRES(is_hdict(H) && k != NULL);

  int i = hashindex(H, k);
  for (chain* p = H->table[i]; p != NULL; p = p->next) {
    if (keyequal(H, entrykey(H, p->entry), k)) {
      return p->entry;
    }
  }

  return NULL;
}

static inline void hdict_resize(hdict* H, size_t new_capacity) {
  REQUIRES(is_hdict(H));
  REQUIRES(new_capacity > 0);

  chain** old_table = H->table;
  chain** new_table = xcalloc(new_capacity, sizeof(chain*));
  size_t old_capacity = H->capacity;
  H->capacity = new_capacity; // is_hdict now violated!

  for (size_t i = 0; i < old_capacity; i++) {
    chain* p_next;
    for (chain* p = old_table[i]; p != NULL; p = p_next) {
      hdict_key k = entrykey(H, p->entry);
      p_next = p->next;
      size_t h = hashindex(H, k);
      p->next = new_table[h];
      new_table[h] = p;
      p = p_next;
    }
  }

  H->table = new_table;
  free(old_table);
}

hdict_entry hdict_insert(hdict* H, hdict_entry e) {
  REQUIRES(is_hdict(H) && e != NULL);

  hdict_key k = entrykey(H, e);
  int i = hashindex(H, k);
  for (chain* p = H->table[i]; p != NULL; p = p->next) {
    if (keyequal(H, entrykey(H, p->entry), k)) {
      hdict_entry old = p->entry;
      p->entry = e;

      ENSURES(is_hdict(H));
      ENSURES(hdict_lookup(H, k) == e);
      return old;
    }
  }

  // prepend new key
  chain* p = xmalloc(sizeof(chain));
  p->entry = e;
  p->next = H->table[i];
  H->table[i] = p;
  (H->size)++;

  if (H->size > H->capacity && H->capacity < UINT_MAX / 2) {
    hdict_resize(H, 2*H->capacity);
  }

  ENSURES(is_hdict(H));
  ENSURES(hdict_lookup(H, k) == e);
  return NULL;
}

void hdict_print(hdict* H,
                 key_print_fn* print_key, entry_print_fn* print_entry) {
  REQUIRES(is_hdict(H));
  REQUIRES(print_key != NULL && print_entry != NULL);

  for(size_t i = 0; i < H->capacity; i++)
    for (chain* p = H->table[i]; p != NULL; p=p->next) {
      (*print_key)(entrykey(H, p->entry));
      fflush(stdout);
      printf(" => ");
      (*print_entry)(p->entry);
      printf("\n");
    }
}

void hdict_free(hdict* H) {
  REQUIRES(is_hdict(H));

  for (size_t i = 0; i < H->capacity; i++) {
    chain* p_next;
    for (chain* p = H->table[i]; p != NULL; p = p_next) {
      if (H->entry_free != NULL) (*H->entry_free)(p->entry);
      p_next = p->next;
      free(p);
    }
  }

  free(H->table);
  free(H);
}
