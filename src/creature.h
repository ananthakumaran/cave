#ifndef __creature_h
#define __creature_h

#include <curses.h>
#include <world.h>

struct CreatureAi;

typedef struct Creature {
  int x;
  int y;
  char glyph;
  int color;
  World *world;
  struct CreatureAi *ai;
} Creature;


void Creature_draw(Creature *creature);
void Creature_move_by(Creature *creature, int x, int y);

Creature *Creature_player_create(World *world);
Creature *Creature_fungus_create(World *world);

#endif
