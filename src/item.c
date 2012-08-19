#include <stdlib.h>
#include "item.h"
#include "dbg.h"

static Item *Item_create(Point *point, char *name, Tile tile)
{
  Item *item = malloc(sizeof(Item));
  die(item, "Out of memory.");

  item->point = point;
  item->name = name;
  item->tile = tile;
  item->owner = NULL;
  item->attack_value = 0;
  item->defense_value = 0;
  item->food_value = 0;
  return item;
}

static Item *Item_weapon_create(Point *point, char *name, Tile tile, int attack_value, int defense_value)
{
  Item *item = Item_create(point, name, tile);
  item->attack_value = attack_value;
  item->defense_value = defense_value;
  return item;
}

static Item *Item_food_create(Point *point, char *name, Tile tile, int food_value)
{
  Item *item = Item_create(point, name, tile);
  item->food_value = food_value;
  return item;
}

Item *Item_create_apple(Point *point)
{
  return Item_food_create(point, "apple", APPLE, 10);
}

Item *Item_create_rock(Point *point)
{
  return Item_weapon_create(point, "rock", ROCK, 2, 0);
}

Item *Item_create_dagger(Point *point)
{
  return Item_weapon_create(point, "dagger", DAGGER, 5, 0);
}

Item *Item_create_sword(Point *point)
{
  return Item_weapon_create(point, "sword", SWORD, 10, 0);
}

Item *Item_create_staff(Point *point)
{
  return Item_weapon_create(point, "staff", STAFF, 5, 3);
}

Item *Item_create_tunic(Point *point)
{
  return Item_weapon_create(point, "tunic", TUNIC, 0, 2);
}

Item *Item_create_chainmail(Point *point)
{
  return Item_weapon_create(point, "chainmail", CHAIN_MAIL, 0, 4);
}

Item *Item_create_platemail(Point *point)
{
  return Item_weapon_create(point, "platemail", PLATE_MAIL, 0, 6);
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
