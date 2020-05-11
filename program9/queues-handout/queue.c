#include <stdlib.h> // Standard C library: free(), NULL...
#include <stdbool.h> // Standard true, false, and bool type
#include "lib/contracts.h" // Our contracts library
#include "lib/xalloc.h" // Our allocation library
#include "queue.h" // The queue interface


typedef struct list_node list;
struct list_node {
  void* data;
  list* next;
};

typedef struct queue_header queue;
struct queue_header {
  list* front;
  list* back;
  int size;
};

bool is_inclusive_segment(list* start, list* end, int i) {
  /* FIX ME */
  if (start == NULL && i == 0) return true;
  else if (i != 0 && start == NULL) return false;
  return is_inclusive_segment(start->next, end, i-1);
}

bool is_queue(queue* Q) {
  /* FIX ME */
  return Q != NULL && is_inclusive_segment(Q->front, Q->back, Q->size);
}

/* Other functions go there */

queue* queue_new()
//@ensures is_queue(\result);
{
  queue *Q = xmalloc(sizeof(queue));
  Q->front = NULL;
  Q->back = NULL;
  Q->size = 0;
  ENSURES(is_queue(Q));
  return Q;
}

size_t queue_size(queue* Q)
//@requires Q != NULL;
//@ensures \result >= 0;
{
  REQUIRES(Q != NULL && is_queue(Q));
  ENSURES(Q->size >= 0);
  return Q->size;
}

void enq(queue* Q, void* x)
//@requires Q != NULL;
{
  REQUIRES(Q != NULL && is_queue(Q));
  list *y = xmalloc(sizeof(list));
  y->next = NULL;
  y->data = x;
  if (queue_size(Q) == 0)
  {
    Q->front = y;
    Q->back = y;
  }
  else
  {
    Q->back->next = y;
    Q->back = y;
  }
  Q->size++;
}

void* deq(queue* Q)
//@requires Q != NULL && queue_size(Q) > 0;
{
  REQUIRES(Q != NULL && queue_size(Q) > 0 && is_queue(Q));
  void *res = Q->front->data;
  Q->front = Q->front->next;
  Q->size = Q->size - 1;
  ENSURES(is_queue(Q));
  return res;
}

void* queue_peek(queue* Q, size_t i)
//@requires Q!= NULL && 0 <= i && i < queue_size(Q);
{
  REQUIRES(Q != NULL && i < queue_size(Q) && is_queue(Q));
  list *start = Q->front;
  if (i == 0) return Q->front->data;
  if (queue_size(Q) == 0) return NULL;
  for (size_t j = 0; j < i; j++)
  {
    start = start->next;
  }
  return start->data;
}

void queue_reverse(queue* Q)
//@requires Q != NULL;
{
  REQUIRES(Q != NULL && is_queue(Q));
  list *head = Q->front;
  list *prev = NULL;
  list *curr = Q->front;;
  list *next = NULL;

  while (curr != NULL)
  {
    next = curr->next;
    curr->next = prev;
    prev = curr;
    curr = next;
  }
  
  Q->front = prev;
  Q->back = head;
  //free(head);
}

bool queue_all(queue* Q, check_property_fn* P)
//@requires Q != NULL;
{
  REQUIRES(Q != NULL && is_queue(Q) && P != NULL);
  if (queue_size(Q) == 0) return true;
  for (size_t i = 0; i < queue_size(Q); i++)
  {
    if ((*P)(queue_peek(Q,i)) == false) return false;
  }
  return true;
}

void* queue_iterate(queue* Q, void* base, iterate_fn* F)
//@requires Q != NULL && F != NULL;
{
  REQUIRES(Q != NULL && F != NULL && is_queue(Q));
  list *curr = Q->front;
  while (curr != Q->back)
  {
    base = (*F) (base, curr->data);
    curr = curr->next;
  }
  //free(curr);
  return base;
}

void queue_free(queue* Q, free_fn *F)
{
  REQUIRES(Q != NULL);
  if (F == NULL)
  {
    list *internals = Q->front;
    while (internals != Q->back)
    {
      list *tmp = internals;
      internals = internals->next;
      free(tmp);
    }
    //free(internals);
  }
  else
  {
    list *curr = Q->front;
    while (curr != Q->back);
    {
      curr = curr->next;
      (*F)(curr);
    }
    //free(curr);
  }
  free(Q);
}