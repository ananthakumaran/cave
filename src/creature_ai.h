#ifndef __creature_ai_h
#define __creature_ai_h

#include <creature.h>

typedef struct CreatureAi CreatureAi;

typedef void (*CreatureAi_enter)(CreatureAi *ai, int x, int y);

struct CreatureAi {
  Creature *creature;
  CreatureAi_enter enter;
};

CreatureAi *CreatureAi_player_create(Creature *player);

#endif
