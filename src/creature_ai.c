#include <stdlib.h>
#include "creature_ai.h"
#include "creature.h"
#include "tile.h"
#include "dbg.h"

static CreatureAi *CreatureAi_create(Creature *creature, CreatureAi_enter enter, CreatureAi_tick tick)
{
  CreatureAi *ai = malloc(sizeof(CreatureAi));
  ai->creature = creature;
  ai->enter = enter;
  ai->tick = tick;

  return ai;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static void CreatureAi_fungus_enter(CreatureAi *ai, int x, int y)
{
// noop
}

static void CreatureAi_player_tick(CreatureAi *ai)
{
// noop
}

#pragma GCC diagnostic pop

static void CreatureAi_player_enter(CreatureAi *ai, int x, int y)
{
  Tile tile = World_tile(ai->creature->world, x, y);
  Creature *creature = ai->creature;

  Creature *other = World_creature_at(ai->creature->world, x, y);

  if(other) {
    // noop
  } else {
    if(Tile_is_ground(tile)) {
      creature->x = x;
      creature->y = y;
    } else if(Tile_is_diggable(tile)) {
      World_dig(creature->world, x, y);
      creature->x = x;
      creature->y = y;
    }
  }
}

CreatureAi *CreatureAi_player_create(Creature *player)
{
  return CreatureAi_create(player, CreatureAi_player_enter, CreatureAi_player_tick);
}

#define FUNGI_SPREAD_DISTANCE 10

static void fungus_spread(CreatureAi *fungus_ai)
{
  Creature *child;
  World *world = fungus_ai->creature->world;
  int x = fungus_ai->creature->x + (rand() % FUNGI_SPREAD_DISTANCE) - FUNGI_SPREAD_DISTANCE / 2;
  int y = fungus_ai->creature->y + (rand() % FUNGI_SPREAD_DISTANCE) - FUNGI_SPREAD_DISTANCE / 2;

  if(World_can_enter(world, x, y)) {
    child = Creature_fungus_create(world);
    World_add_creature(world, child);
    child->x = x;
    child->y = y;
    fungus_ai->spread_count++;
  }
}

void CreatureAi_fungus_tick(CreatureAi *fungus_ai)
{
  if(fungus_ai->spread_count < 5 && rand() % 10000 == 0) {
    fungus_spread(fungus_ai);
  }
}

CreatureAi *CreatureAi_fungus_create(Creature *fungus)
{
  CreatureAi *fungus_ai = CreatureAi_create(fungus, CreatureAi_fungus_enter, CreatureAi_fungus_tick);
  fungus_ai->spread_count = 0;
  return fungus_ai;
}
