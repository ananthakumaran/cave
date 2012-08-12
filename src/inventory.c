#include "inventory.h"
#include "dbg.h"

int Inventory_add(Creature *creature, Item *item)
{
  int i = 0;

  for(i = 0; i < creature->inventory_size; i++) {
    if(creature->inventory[i] == NULL) {
      creature->inventory[i] = item;
      item->owner = creature;
      return 1;
    }
  }

  return 0;
}

int Inventory_remove(Creature *creature, Item *item)
{
  int i = 0;

  for(i = 0; i < creature->inventory_size; i++) {
    if(creature->inventory[i] == item) {
      creature->inventory[i] = NULL;
      item->owner = NULL;
      return 1;
    }
  }

  return 0;
}

int Inventory_is_full(Creature *creature)
{
  int i = 0;

  for(i = 0; i < creature->inventory_size; i++) {
    if(creature->inventory[i] == NULL) {
      return 0;
    }
  }

  return 1;
}
