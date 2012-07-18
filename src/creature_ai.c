#include <stdlib.h>
#include "creature_ai.h"
#include "creature.h"
#include "tile.h"
#include "dbg.h"

static CreatureAi *CreatureAi_create(Creature *creature, CreatureAi_enter enter)
{
  CreatureAi *ai = malloc(sizeof(CreatureAi));
  ai->creature = creature;
  ai->enter = enter;

  return ai;
}

static void CreatureAi_player_enter(CreatureAi *ai, int x, int y)
{
  Tile tile = World_tile(ai->creature->world, x, y);
  Creature *creature = ai->creature;

  if(Tile_is_ground(tile)) {
    creature->x = x;
    creature->y = y;
  } else if(Tile_is_diggable(tile)) {
    World_dig(creature->world, x, y);
    creature->x = x;
    creature->y = y;
  }
}

CreatureAi *CreatureAi_player_create(Creature *player)
{
  return CreatureAi_create(player, CreatureAi_player_enter);
}
