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

Creature *Creature_player_create(World *world);

void Creature_draw(Creature *creature);

void Creature_move_by(Creature *creature, int x, int y);

#endif
