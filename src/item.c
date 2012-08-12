#include <stdlib.h>
#include "item.h"
#include "dbg.h"

Item *Item_create_rock(Point *point)
{
  Item *item = malloc(sizeof(Item));
  die(item, "Out of memory.");

  item->point = point;
  item->name = "rock";
  item->tile = ROCK;
  item->owner = NULL;

  return item;
}

void Item_set_point(Item *item, Point *point)
{
  if(item->point) free(item->point);

  item->point = point;
}

void Item_destroy(Item *item)
{
  free(item->point);
  free(item);
}
