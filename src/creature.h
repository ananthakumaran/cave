#ifndef __creature_h
#define __creature_h

#include <curses.h>
#include <world.h>

struct CreatureAi;

typedef struct Creature {
  int x;
  int y;
  int z;
  char glyph;
  int color;

  int hit_point;
  int attack_value;
  int defense_value;
  int vision_radius;

  World *world;
  struct CreatureAi *ai;
} Creature;


void Creature_destroy(Creature *creature);
void Creature_draw(Creature *creature);
void Creature_move_by(Creature *creature, int x, int y, int z);

Creature *Creature_player_create(World *world);
Creature *Creature_fungus_create(World *world);

#endif
