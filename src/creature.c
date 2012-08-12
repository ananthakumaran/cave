#include <stdlib.h>
#include "creature.h"
#include "item.h"
#include "creature_ai.h"
#include "world.h"
#include "utils.h"
#include "dbg.h"

static Creature* Creature_create(World *world, char glyph, int color, int inventory_size)
{
  Creature *creature = malloc(sizeof(Creature) + inventory_size * sizeof(Item *));
  die(creature, "Could not create creature");

  creature->world = world;
  creature->glyph = glyph;
  creature->color = color;
  creature->x = -1;
  creature->y = -1;
  creature->z = -1;

  creature->hit_point = 0;
  creature->attack_value = 0;
  creature->defense_value = 0;
  creature->vision_radius = 0;
  creature->inventory_size = inventory_size;

  int i = 0;
  for(i = 0; i < creature->inventory_size; i++) {
    creature->inventory[i] = NULL;
  }

  return creature;
}

void Creature_destroy(Creature *creature)
{
  CreatureAi_destroy(creature->ai);
  free(creature);
}


void Creature_draw(Creature *creature)
{
  World *world = creature->world;

  if(World_is_in_view(world, creature->x, creature->y)) {
    move(creature->y - world->screen_top , creature->x - world->screen_left);
    attrset(COLOR_PAIR(creature->color));
    addch(NCURSES_ACS(creature->glyph));
  }
}

void Creature_move_by(Creature *creature, int x, int y, int z)
{
  creature->ai->enter(creature->ai, creature->x + x, creature->y + y, creature->z + z);
}

Creature *Creature_player_create(World *world)
{
  Creature *player = Creature_create(world, '{', 4, 20);
  player->hit_point = 100;
  player->attack_value = 20;
  player->defense_value = 5;
  player->vision_radius = 10;
  player->ai = CreatureAi_player_create(player);
  return player;
}

Creature *Creature_fungus_create(World *world)
{
  Creature *fungus = Creature_create(world, '}', 3, 0);
  fungus->hit_point = 10;
  fungus->attack_value = 1;
  fungus->defense_value = 0;
  fungus->ai = CreatureAi_fungus_create(fungus);
  return fungus;
}
