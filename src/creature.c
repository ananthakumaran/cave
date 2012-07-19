#include <stdlib.h>
#include "creature.h"
#include "creature_ai.h"
#include "world.h"
#include "utils.h"
#include "dbg.h"

static Creature* Creature_create(World *world, char glyph, int color)
{
  Creature *creature = malloc(sizeof(Creature));
  die(creature, "Could not create creature");

  creature->world = world;
  creature->glyph = glyph;
  creature->color = color;
  creature->x = -1;
  creature->y = -1;

  return creature;
}


void Creature_draw(Creature *creature)
{
  World *world = creature->world;

  if(WITHIN(creature->x, world->screen_left, world->screen_left + world->screen_width) &&
     WITHIN(creature->y, world->screen_top, world->screen_top + world->screen_height)) {

    move(creature->y - world->screen_top , creature->x - world->screen_left);
    attrset(COLOR_PAIR(creature->color));
    addch(NCURSES_ACS(creature->glyph));
  }
}

void Creature_move_by(Creature *creature, int x, int y)
{
  creature->ai->enter(creature->ai, creature->x + x, creature->y + y);
}

Creature *Creature_player_create(World *world)
{
  Creature *player = Creature_create(world, '{', 4);
  player->ai = CreatureAi_player_create(player);
  return player;
}

Creature *Creature_fungus_create(World *world)
{
  Creature *fungus = Creature_create(world, '}', 3);
  fungus->ai = CreatureAi_fungus_create(fungus);
  return fungus;
}
