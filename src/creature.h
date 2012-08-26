#ifndef __creature_h
#define __creature_h

#include <curses.h>
#include "world.h"
#include "item.h"

struct CreatureAi;

typedef struct Creature {
  int x;
  int y;
  int z;
  char glyph;
  int color;
  char *name;

  int hit_point;
  int attack_value;
  int defense_value;
  int vision_radius;

  Item *armor;
  Item *weapon;

  World *world;
  struct CreatureAi *ai;

  int inventory_size;
  Item *inventory[];
} Creature;


void Creature_destroy(Creature *creature);
void Creature_draw(Creature *creature);
void Creature_move_by(Creature *creature, int x, int y, int z);
Point *Creature_location(Creature *creature);

Creature *Creature_player_create(World *world);
Creature *Creature_fungus_create(World *world);
Creature *Creature_apple_tree_create(World *world);
Creature *Creature_zombie_create(World *world);

#endif
