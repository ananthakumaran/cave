#ifndef _world_h
#define _world_h

#include "tile.h"
#include "list.h"

struct Creature;
struct Item;

typedef struct Message {
  char *msg;
  int life;
  int free_msg;
} Message;

typedef struct Point {
  int x;
  int y;
  int z;
} Point;

Point *Point_create(int x, int y, int z);

typedef struct World {
  int width;
  int height;
  int depth;
  int screen_height;
  int screen_width;
  int screen_top;
  int screen_left;
  Tile ***tiles;
  int ***regions;
  int region_count;
  struct Creature *player;
  List *creatures;
  List *items;
  List *messages;
} World;

#define SCREEN_HEIGHT 40
#define SCREEN_WIDTH 75

#define WORLD_HEIGHT 110
#define WORLD_WIDTH 100
#define WORLD_DEPTH 10

Tile World_tile(World *world, int x, int y, int z);
char World_glyph(World *world, int x, int y, int z);
int World_color(World *world, int x, int y, int z);

World *World_create();
void World_destroy(World *world);
void World_tick(World *world);
void World_center_by(World *world, int x, int y);
void World_dig(World *world, int x, int y, int z);
int World_can_enter(World *world, int x, int y, int z);
int World_is_in_view(World *world, int x, int y);

Point *World_get_empty_location(World *world);
void World_add_at_empty_location(World *world, struct Creature *creature);
void World_add_creature(World *world, struct Creature *creature);
void World_remove_creature(World *world, struct Creature *creature);
void World_add_item(World *world, struct Item *item);
void World_notify(World *world, char *message, int free_msg);

struct Creature *World_creature_at(World *world, int x, int y, int z);
struct Item *World_item_at(World *world, int x, int y, int z);

#endif
