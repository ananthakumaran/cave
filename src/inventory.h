#ifndef _inventory_h
#define _inventory_h

#include "creature.h"
#include "item.h"

int Inventory_add(Creature *creature, Item *item);
int Inventory_remove(Creature *creature, Item *item);
int Inventory_is_full(Creature *creature);

#endif
