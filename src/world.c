#include <stdlib.h>
#include "world.h"
#include "creature.h"
#include "tile.h"
#include "dbg.h"
#include "utils.h"

Tile World_tile(World *world, int x, int y)
{
    if(x < 0 || x >= world->width || y < 0 || y >= world->height) {
      return BOUNDS;
    } else {
      return world->tiles[y][x];
    }
}

char World_glyph(World *world, int x, int y)
{
  return World_tile(world, x, y).glyph;
}

int World_color(World *world, int x, int y)
{
  return World_tile(world, x, y).color;
}

World *World_create()
{
  World *world = malloc(sizeof(World));
  int i = 0, j = 0;

  int height = WORLD_HEIGHT;
  int width = WORLD_WIDTH;

  world->tiles = Tile_create(width, height);

  for(i = 0; i < height; i++) {
    for(j = 0; j < width; j++) {
      world->tiles[i][j] = rand() % 2 == 1 ? WALL : FLOOR;
    }
  }

  world->width = width;
  world->height = height;
  world->screen_height = SCREEN_HEIGHT;
  world->screen_width = SCREEN_WIDTH;

  world->tiles = Tile_smooth(world->tiles, 8, width, height);

  world->screen_top = 0;
  world->screen_left = 0;

  return world;
}

void World_destroy(World *world)
{
  Tile_destroy(world->tiles, world->height);
  free(world);
}

void World_center_by(World *world, int x, int y)
{
  int new_left = MAX(0, MIN(x - world->screen_width / 2, world->width - world->screen_width - 1));
  int new_top = MAX(0, MIN(y - world->screen_height / 2, world->height - world->screen_height - 1));

  if(new_left + world->screen_width < world->width && new_left >= 0) {
    world->screen_left = new_left;
  }

  if(new_top + world->screen_height < world->height && new_top >= 0) {
    world->screen_top = new_top;
  }
}

void World_dig(World *world, int x, int y)
{
  if(Tile_is_diggable(World_tile(world, x, y))) {
    world->tiles[y][x] = FLOOR;
  }
}

void World_add_at_empty_location(World *world, Creature *creature)
{
  int x = 0, y = 0;

  do {
    x = rand() % SCREEN_WIDTH;
    y = rand() % SCREEN_HEIGHT;
  } while(!Tile_is_ground(World_tile(world, x,y)));

  creature->x = x;
  creature->y = y;
}