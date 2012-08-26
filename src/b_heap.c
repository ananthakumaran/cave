#include "b_heap.h"
#include "dbg.h"


BHeap *BHeap_create(BHeap_compare compare, size_t init_size)
{
  check(init_size > 0, "init_size should be greater than 1.")

  BHeap *b_heap = malloc(sizeof(BHeap));
  check_mem(b_heap);

  b_heap->size = 0;
  b_heap->allocated = init_size;
  b_heap->compare = compare;

  b_heap->items = malloc(sizeof(void *) * init_size);
  check_mem(b_heap->items);

  return b_heap;

 error:
  if(b_heap) BHeap_destroy(b_heap);
  return NULL;
}

void BHeap_destroy(BHeap *b_heap)
{
  if(b_heap) {
    free(b_heap->items);
    free(b_heap);
  }
}

static int BHeap_expand(BHeap *b_heap)
{
  void *new_items = realloc(b_heap->items,
			    sizeof(void *) * (b_heap->allocated + BHEAP_ENLARGE_SIZE));

  check_mem(new_items);
  b_heap->items = new_items;
  b_heap->allocated += BHEAP_ENLARGE_SIZE;
  return 1;

 error:
  return -1;
}

int BHeap_insert(BHeap *b_heap, void *item)
{
  if(b_heap->size == b_heap->allocated) {
    check(BHeap_expand(b_heap) != -1, "BHeap expansion failed.");
  }


  void **items = b_heap->items;
  items[b_heap->size] = item;

  b_heap->size++;

  int i = b_heap->size - 1;
  int parent = (i - 1) / 2;
  void *temp;

  while(parent >= 0 &&
	b_heap->compare(items[parent], items[i]) > 0) {

    temp = items[parent];
    items[parent] = items[i];
    items[i] = temp;

    i = parent;
    parent = (i - 1) / 2;
  }

  return 1;
 error:
  return -1;
}

void *BHeap_pop(BHeap *b_heap)
{
  if(b_heap->size == 0) return NULL;

  void **items = b_heap->items;
  void *top = items[0];

  items[0] = items[b_heap->size - 1];
  b_heap->size--;

  size_t i = 0;
  size_t size = b_heap->size;

  while(1) {

    size_t left = 2 * i + 1;
    size_t smallest = i;
    size_t right = 2 * i + 2;

    if(left < size &&
       b_heap->compare(items[smallest], items[left]) > 0) {
      smallest = left;
    }

    if(right < size &&
       b_heap->compare(items[smallest], items[right]) > 0) {
      smallest = right;
    }

    if(smallest != i) {
      void *temp = items[i];
      items[i] = items[smallest];
      items[smallest] = temp;
      i = smallest;
    } else {
      break;
    }
  }

  return top;
}

void *BHeap_peek(BHeap *b_heap)
{
  if(b_heap->size == 0) {
    return NULL;
  }

  return b_heap->items[0];
}
