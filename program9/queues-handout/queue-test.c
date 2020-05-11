#include <stdlib.h> // Standard C library: free(), NULL...
#include <stdbool.h> // Standard true, false, and bool type
#include "lib/contracts.h" // Our contracts library
#include "lib/xalloc.h" // Our allocation library
#include "queue.h" // The queue interface

int main() {
  // Create a few queues
  printf("hello\n");
  queue_t Q = queue_new();
  int *p1 = xmalloc(sizeof(int));
  *p1 = 8;
  int *p2 = xmalloc(sizeof(int));
  *p2 = 50;
  int *p3  = xmalloc(sizeof(int));
  *p3 = 22;
  int *p4  = xmalloc(sizeof(int));
  *p4 = 37;
  int *p5  = xmalloc(sizeof(int));
  *p5 = 72;
  enq(Q, (void*)p1);
  enq(Q, (void*)p2);
  enq(Q, (void*)p3);
  enq(Q, (void*)p4);
  enq(Q, (void*)p5);
  assert(queue_size(Q) == 5);
  printf("got the queue all filled up\n");
  assert(*(int*)queue_peek(Q, 0) == 8);
  assert(*(int*)queue_peek(Q, 1) == 50);
  assert(*(int*)queue_peek(Q, 2) == 22);
  assert(*(int*)queue_peek(Q, 3) == 37);
  assert(*(int*)queue_peek(Q, 4) == 72);

  queue_reverse(Q);
  printf("reversed the queue\n");
  assert(*(int*)queue_peek(Q, 0) == 72);
  assert(*(int*)queue_peek(Q, 1) == 37);
  assert(*(int*)queue_peek(Q, 2) == 22);
  assert(*(int*)queue_peek(Q, 3) == 50);
  assert(*(int*)queue_peek(Q, 4) == 8);

  assert(*(int*)deq(Q) == 72);

  free(p1);
  free(p2);
  free(p3);
  free(p4);
  free(p5);

  printf("\nAll tests passed!");
  return 0;
}