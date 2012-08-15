#ifndef _item_h
#define _item_h

#include "tile.h"
#include "world.h"

struct Creature;

typedef struct Item {
  Tile tile;
  char *name;
  Point *point;
  struct Creature *owner;
} Item;

Item *Item_create_rock(Point *point);
Item *Item_create_apple(Point *point);
void Item_set_point(Item *item, Point *point);

void Item_destroy(Item *item);

#endif
