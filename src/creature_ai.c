#include <stdlib.h>
#include "creature_ai.h"
#include "creature.h"
#include "tile.h"
#include "utils.h"
#include "dbg.h"

static void CreatureAi_hit_default(CreatureAi *ai, int power)
{
  ai->creature->hit_point -= power;
  if(ai->creature->hit_point < 1) {
    World_remove_creature(ai->creature->world, ai->creature);
  }
}

static CreatureAi *CreatureAi_create(Creature *creature, CreatureAi_enter enter, CreatureAi_tick tick, CreatureAi_hit hit)
{
  CreatureAi *ai = malloc(sizeof(CreatureAi));
  ai->creature = creature;
  ai->enter = enter;
  ai->tick = tick;

  if(hit) {
    ai->hit = hit;
  } else {
    ai->hit = CreatureAi_hit_default;
  }

  return ai;
}

void CreatureAi_destroy(CreatureAi *ai)
{
  free(ai);
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

static void CreatureAi_player_attack(Creature *player, Creature *creature)
{
  int amount = MAX(1, player->attack_value - creature->defense_value);
  amount = rand() % amount;
  creature->ai->hit(creature->ai, amount);
  player->hit_point -= 1;

  World_notify(player->world, "You have attacked a fungus");
}

static void CreatureAi_player_enter(CreatureAi *ai, int x, int y)
{
  Tile tile = World_tile(ai->creature->world, x, y);
  Creature *creature = ai->creature;

  Creature *other = World_creature_at(ai->creature->world, x, y);

  if(other) {
    CreatureAi_player_attack(ai->creature, other);
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
  return CreatureAi_create(player, CreatureAi_player_enter, CreatureAi_player_tick, NULL);
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
  if(fungus_ai->spread_count < 5 && rand() % 5000 == 0) {
    fungus_spread(fungus_ai);
  }
}

CreatureAi *CreatureAi_fungus_create(Creature *fungus)
{
  CreatureAi *fungus_ai = CreatureAi_create(fungus, CreatureAi_fungus_enter, CreatureAi_fungus_tick, NULL);
  fungus_ai->spread_count = 0;
  return fungus_ai;
}
