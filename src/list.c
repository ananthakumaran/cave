#include "list.h"
#include "dbg.h"

List *List_create()
{
  return calloc(1, sizeof(List));
}

List *List_copy(List *list)
{
  List *copy = List_create();

  LIST_FOREACH(list, first, next, cur) {
    List_push(copy, cur->value);
  }

  return copy;
}

void List_destroy(List *list)
{
  LIST_FOREACH(list, first, next, cur) {
    if(cur->prev) {
      free(cur->prev);
    }
  }

  free(list->last);
  free(list);

}

void List_clear(List *list)
{
  LIST_FOREACH(list, first, next, curr) {
    free(curr->value);
  }
}

void List_clear_destroy(List *list)
{
  List_clear(list);
  List_destroy(list);
}

void List_push(List *list, void *value)
{
  ListNode *node = calloc(1, sizeof(ListNode));
  check_mem(node);

  node->value = value;

  if(list->last == NULL) {
    list->first = node;
    list->last = node;
  } else {
    node->prev = list->last;
    list->last->next = node;
    list->last = node;
  }

  list->count++;
 error:
  return;
}

void* List_pop(List *list)
{
  ListNode *node = list->last;
  return node != NULL ? List_remove(list, node) : NULL;
}

void List_shift(List *list, void *value)
{
  ListNode *node = calloc(1, sizeof(ListNode));
  check_mem(node);

  node->value = value;

  if(list->first == NULL) {
    list->first = node;
    list->last = node;
  } else {
    node->next = list->first;
    list->first->prev = node;
    list->first = node;
  }

  list->count++;
 error:
  return;
}

void* List_unshift(List *list)
{
  ListNode *node = list->first;
  return node != NULL ? List_remove(list, node) : NULL;
}

int List_contains(List *list, void *value)
{
  LIST_FOREACH(list, first, next, cur) {
    if(cur->value == value) return 1;
  }

  return 0;
}

int List_delete(List *list, void *value)
{

  LIST_FOREACH(list, first, next, cur) {
    if(cur->value == value) {
      List_remove(list, cur);
      return 1;
    }
  }

  return 0;
}

void List_shuffle(List *list)
{
  int times = List_count(list);
  int i;
  ListNode *random_node;
  void *temp;


  while(times--) {

    i = rand() % List_count(list);

    LIST_FOREACH(list, first, next, cur) {
      if(!i--) {
	random_node = cur;
	break;
      }
    }

    temp = list->first->value;
    list->first->value = random_node->value;
    random_node->value = temp;

  }
}


void* List_remove(List *list, ListNode *node)
{
  void *result = NULL;

  check(list->first && list->last, "List is empty.");
  check(node, "node can't be NULL");

  if(node == list->first && node == list->last) {
    list->first = NULL;
    list->last = NULL;
  } else if(node == list->first) {
    list->first = node->next;
    check(list->first != NULL, "Invalid list, somehow got a first that is NULL");
    list->first->prev = NULL;
  } else if(node == list->last) {
    list->last = node->prev;
    check(list->last != NULL, "Invalid list, somehow got a last that is NULL");
    list->last->next = NULL;
  } else {
    ListNode *after = node->next;
    ListNode *before = node->prev;
    after->prev = before;
    before->next = after;
  }

  list->count--;
  result = node->value;
  free(node);

 error:
  return result;
}
