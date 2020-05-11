#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "lib/xalloc.h"
#include "lib/contracts.h"
#include "lib/bitvector.h"
#include "board-ht.h"
#include "lib/queue.h"
#include "lib/hdict.h"
#include "lib/boardutil.h"

typedef struct board_data DAT;

bitvector press_button(int row, int col, uint8_t height, uint8_t width, bitvector b)
{
  int index = get_index(row, col, width, height);
  bitvector flipped = bitvector_flip(b, index);

  //check if there are bits N, E, S, W flippable
  // NORTH
  if ((row+1) < height)
  {
  	index = get_index(row+1, col, width, height);
  	flipped = bitvector_flip(flipped, index);
  }
  // EAST
  if ((col+1) < width)
  {
  	index = get_index(row, col+1, width, height);
  	flipped = bitvector_flip(flipped, index);
  }
  // SOUTH
  if ((row-1) >= 0)
  {
  	index = get_index(row-1, col, width, height);
  	flipped = bitvector_flip(flipped, index);
  }
  // WEST
  if ((col-1) >= 0)
  {
  	index = get_index(row, col-1, width, height);
  	flipped = bitvector_flip(flipped, index);
  }
  return flipped;
}



int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: lightsout <board name>\n");
    return 1;
  }
  char *board_filename = argv[1];
  
  queue_t Q = queue_new();
  hdict_t H = ht_new((size_t) 100);
  uint8_t height = 0;
  uint8_t width = 0;
  bitvector vec_out = bitvector_new();

  if (!file_read(board_filename, &vec_out, &width, &height))
  {
    fprintf(stderr, "cannot read board");
    queue_free(Q, NULL);
    hdict_free(H);
    return 1;
  }

  DAT *tmp_board = xmalloc(sizeof(DAT));
  tmp_board->board = vec_out;
  int index = 0;
  tmp_board->index = index;

  ht_insert(H, tmp_board);
  enq(Q, (void*) tmp_board);

  while (!queue_empty(Q)) 
  {
  	DAT *B = (DAT*) deq(Q);
    vec_out = B->board;
    index = B->index;

  	for (int row = 0; row < height; row++) {
  	  for (int col = 0; col < width; col++) {
        vec_out = press_button(row, col, height, width, B->board);
        //index = get_index(row, col, width, height);
  	  	if (bitvector_equal(vec_out, bitvector_new()))
  	  	{
          //DAT *res = ht_lookup(H, vec_out);

  	  	  queue_free(Q, NULL);
  	  	  hdict_free(H);
  	  	  //fprintf(stderr, "board could be solved");
  	  	  return 0;
  	  	}

  	  	if (ht_lookup(H, vec_out) == NULL)
  	  	{
  	  	  DAT *N = xmalloc(sizeof(DAT));
  	  	  N->board = vec_out;
          N->index = index;
  	  	  ht_insert(H, N);
  	  	  enq(Q, (void*) N);
  	  	}
  	  }
  	}
  }
  queue_free(Q, NULL);
  hdict_free(H);
  fprintf(stderr, "board could not be solved");
}