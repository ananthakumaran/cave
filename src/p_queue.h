#ifndef _p_queue_h
#define _p_queue_h

#include <stdint.h>
#include "hashmap.h"
#include "b_heap.h"

typedef int(*PQueue_compare)(void *a, void *b);
typedef uint32_t (*PQueue_hash)(void *item);

typedef struct PQueueNode {
  int priority;
  void *item;
} PQueueNode;

typedef struct PQueue {
  Hashmap *map;
  BHeap *heap;
} PQueue;

PQueue *PQueue_create(PQueue_compare compare, PQueue_hash hash);
void PQueue_destroy(PQueue *p_queue);
void *PQueue_pop(PQueue *p_queue);
void *PQueue_peek(PQueue *p_queue);
int PQueue_push(PQueue *p_queue, void *item, int priority);
int PQueue_contains(PQueue *p_queue, void *item);
int PQueue_empty(PQueue *p_queue);

#endif
