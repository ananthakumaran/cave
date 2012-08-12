#ifndef __creature_ai_h
#define __creature_ai_h

#include "creature.h"
#include "item.h"

typedef struct CreatureAi CreatureAi;

typedef void (*CreatureAi_enter)(CreatureAi *ai, int x, int y, int z);
typedef void (*CreatureAi_tick)(CreatureAi *ai);
typedef void (*CreatureAi_pickup)(CreatureAi *ai, int x, int y, int z);
typedef void (*CreatureAi_drop)(CreatureAi *ai, Item *item);
typedef void (*CreatureAi_hit)(CreatureAi *ai, int power);
typedef int (*CreatureAi_can_see)(CreatureAi *ai, int x, int y, int z);

struct CreatureAi {
  Creature *creature;
  CreatureAi_enter enter;
  CreatureAi_tick tick;
  CreatureAi_pickup pickup;
  CreatureAi_drop drop;
  CreatureAi_hit hit;
  CreatureAi_can_see can_see;

  int spread_count; // fungus ai
};

void CreatureAi_destroy(CreatureAi *ai);

CreatureAi *CreatureAi_player_create(Creature *player);
CreatureAi *CreatureAi_fungus_create(Creature *fungus);

#endif
