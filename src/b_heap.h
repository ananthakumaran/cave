#ifndef _b_heap_h
#define _b_heap_h

#include <stdlib.h>

typedef int(*BHeap_compare)(void *a, void *b);

#define BHEAP_ENLARGE_SIZE 10

typedef struct BHeap {
  void **items;
  size_t size;
  size_t allocated;

  BHeap_compare compare;
} BHeap;

BHeap *BHeap_create(BHeap_compare compare, size_t init_size);
int BHeap_insert(BHeap *b_heap, void *item);
void *BHeap_pop(BHeap *b_heap);
void *BHeap_peek(BHeap *b_heap);
void BHeap_destroy(BHeap *b_heap);

#endif
