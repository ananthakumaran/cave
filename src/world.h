#ifndef _world_h
#define _world_h

#include "tile.h"
#include "list.h"

struct Creature;

typedef struct Message {
  char *msg;
  int life;
} Message;

typedef struct World {
  int width;
  int height;
  int screen_height;
  int screen_width;
  int screen_top;
  int screen_left;
  Tile **tiles;
  struct Creature *player;
  List *creatures;
  List *messages;
} World;

#define SCREEN_HEIGHT 40
#define SCREEN_WIDTH 75

#define WORLD_HEIGHT 100
#define WORLD_WIDTH 100

Tile World_tile(World *world, int x, int y);
char World_glyph(World *world, int x, int y);
int World_color(World *world, int x, int y);

World *World_create();
void World_destroy(World *world);
void World_tick(World *world);
void World_center_by(World *world, int x, int y);
void World_dig(World *world, int x, int y);
int World_can_enter(World *world, int x, int y);

void World_add_at_empty_location(World *world, struct Creature *creature);
void World_add_creature(World *world, struct Creature *creature);
void World_remove_creature(World *world, struct Creature *creature);
void World_notify(World *world, char *message);

struct Creature *World_creature_at(World *world, int x, int y);

#endif
