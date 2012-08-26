#include "dbg.h"
#include "p_queue.h"

static int PQueueNode_cmp(void *a, void *b)
{
  return ((PQueueNode *)a)->priority - ((PQueueNode *)b)->priority;
}

PQueue *PQueue_create(PQueue_compare compare, PQueue_hash hash)
{
  PQueue *p_queue = malloc(sizeof(PQueue));
  check_mem(p_queue);

  p_queue->heap = BHeap_create(PQueueNode_cmp, 10);
  check_mem(p_queue->heap);

  p_queue->map = Hashmap_create(compare, hash);
  check_mem(p_queue->map);

  return p_queue;
 error:
  if(p_queue) PQueue_destroy(p_queue);
  return NULL;
}

void PQueue_destroy(PQueue *p_queue)
{
  size_t i;
  if(p_queue) {
    Hashmap_destroy(p_queue->map);

    for(i = 0; i < p_queue->heap->size; i++) {
      free(p_queue->heap->items[i]);
    }

    BHeap_destroy(p_queue->heap);
    free(p_queue);
  }
}

static void *item(PQueueNode *node)
{
  return node != NULL ? node->item : NULL;
}

void *PQueue_pop(PQueue *p_queue)
{
  void *item = NULL;
  PQueueNode *node = BHeap_pop(p_queue->heap);

  if(node) {
    Hashmap_delete(p_queue->map, node->item);
    item = node->item;
    free(node);
  }

  return item;
}

void *PQueue_peek(PQueue *p_queue)
{
  return item(BHeap_peek(p_queue->heap));
}


int PQueue_push(PQueue *p_queue, void *item, int priority)
{
  PQueueNode *node = malloc(sizeof(PQueueNode));
  check_mem(node);
  node->item = item;
  node->priority = priority;

  int rc = Hashmap_set(p_queue->map, item, node);
  check(rc != -1, "Failed to set hash");

  rc = BHeap_insert(p_queue->heap, node);
  if(rc == -1) {
    Hashmap_delete(p_queue->map, item);
    sentinel("Failed to push noe");
  }

  return 1;

 error:
  if(node) free(node);
  return -1;
}

int PQueue_contains(PQueue *p_queue, void *item)
{
  PQueueNode *node = Hashmap_get(p_queue->map, item);
  return node != NULL ? 1 : 0;
}

int PQueue_empty(PQueue *p_queue)
{
  return p_queue->heap->size == 0;
}
