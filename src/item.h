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

  int attack_value;
  int defense_value;

  int food_value;
} Item;

Item *Item_create_rock(Point *point);
Item *Item_create_apple(Point *point);
Item *Item_create_dagger(Point *point);
Item *Item_create_sword(Point *point);
Item *Item_create_staff(Point *point);
Item *Item_create_tunic(Point *point);
Item *Item_create_chainmail(Point *point);
Item *Item_create_platemail(Point *point);

void Item_set_point(Item *item, Point *point);

void Item_destroy(Item *item);

#endif
