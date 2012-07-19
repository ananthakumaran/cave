#ifndef __creature_ai_h
#define __creature_ai_h

#include <creature.h>

typedef struct CreatureAi CreatureAi;

typedef void (*CreatureAi_enter)(CreatureAi *ai, int x, int y);
typedef void (*CreatureAi_tick)(CreatureAi *ai);

struct CreatureAi {
  Creature *creature;
  CreatureAi_enter enter;
  CreatureAi_tick tick;

  int spread_count; // fungus ai
};

CreatureAi *CreatureAi_player_create(Creature *player);
CreatureAi *CreatureAi_fungus_create(Creature *fungus);

#endif
