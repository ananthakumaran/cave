#include <stdlib.h>
#include "world.h"
#include "creature.h"
#include "creature_ai.h"
#include "tile.h"
#include "dbg.h"
#include "utils.h"
#include "list.h"
#include "bitmap.h"

Point *Point_create(int x, int y, int z)
{
  Point *point = malloc(sizeof(Point));
  point->x = x;
  point->y = y;
  point->z = z;

  return point;
}


Tile World_tile(World *world, int x, int y, int z)
{
    if(x < 0 || x >= world->width || y < 0 || y >= world->height || z < 0 || z >= world->depth) {
      return BOUNDS;
    } else {
      return world->tiles[x][y][z];
    }
}

char World_glyph(World *world, int x, int y, int z)
{
  return World_tile(world, x, y, z).glyph;
}

int World_color(World *world, int x, int y, int z)
{
  return World_tile(world, x, y, z).color;
}

static void World_alloc_regions(World *world)
{
  int i, j, k;
  int ***regions = malloc(world->width * sizeof(int *));
  die(regions, "Out of memory");

  for(i = 0; i < world->width; i++) {
    regions[i] = malloc(world->height * sizeof(int *));
    die(regions[i], "Out of memory");

    for(j = 0; j < world->height; j++) {
      regions[i][j] = malloc(world->depth * sizeof(int));
      die(regions[i][j], "Out of memory");

      for(k = 0; k < world->depth; k++) {
	regions[i][j][k] = 0;
      }
    }
  }

  world->regions = regions;
}

static void World_fill_region(World *world, int region, int x, int y, int z)
{

  int nx, ny;
  Point *point;
  List *open = List_create();
  List_push(open, Point_create(x, y, z));

  while(List_count(open) != 0) {
    point = List_unshift(open);

    for(nx = point->x - 1; nx <= point->x + 1; nx++) {
      if(nx < 0 || nx >= world->width) continue;
      for(ny = point->y - 1; ny <= point->y + 1; ny++) {
	if(ny < 0 || ny >= world->height) continue;

	if(!TILE_EQ(world->tiles[nx][ny][z], WALL) && world->regions[nx][ny][z] == 0) {
	  world->regions[nx][ny][z] = region;
	  List_push(open, Point_create(nx, ny, z));
	}
      }
    }

    free(point);
  }

  List_destroy(open);
}

static List *find_region_overlaps(World *world, int z, int r1, int r2)
{
  List *result = List_create();

  int x, y;

  for(x = 0; x < world->width; x++) {
    for(y = 0; y < world->height; y++) {
      if(TILE_EQ(world->tiles[x][y][z], FLOOR) &&
	 TILE_EQ(world->tiles[x][y][z + 1], FLOOR) &&
	 world->regions[x][y][z] == r1 &&
	 world->regions[x][y][z + 1] == r2) {
	List_push(result, Point_create(x, y, z));
      }
    }
  }

  List_shuffle(result);
  return result;
}

static void World_connect_regions_down(World *world, int z)
{
  int x, y;

  // bad idea?
  Bitmap *is_connected = Bitmap_create(world->region_count * world->region_count);

  int r1, r2, stairs, candidates_size;

  for(x = 0; x < world->width; x++) {
    for(y = 0; y < world->height; y++) {

      r1 = world->regions[x][y][z];
      r2 = world->regions[x][y][z + 1];

      if(TILE_EQ(world->tiles[x][y][z], FLOOR) &&
	 TILE_EQ(world->tiles[x][y][z + 1], FLOOR) &&
	 (!Bitmap_isset(is_connected, r1 * r2))) {

	Bitmap_set(is_connected, r1 * r2);

	List *candidates = find_region_overlaps(world, z, r1, r2);
	stairs = 0;
	candidates_size = List_count(candidates);

	if(candidates_size > 0) {
	  do {
	    Point *p = List_unshift(candidates);
	    world->tiles[p->x][p->y][z] = STAIR_DOWN;
	    world->tiles[p->x][p->y][z+1] = STAIR_UP;
	    free(p);
	    stairs++;
	  } while((candidates_size / stairs > 250) && List_count(candidates));
	}

	List_clear_destroy(candidates);
      }
    }
  }

  Bitmap_destroy(is_connected);
}

static void World_connect_regions(World *world)
{
  int i;

  for(i = 0; i < world->depth - 1; i++) {
    World_connect_regions_down(world, i);
  }
}

static void World_create_regions(World *world)
{
  int x, y, z;
  World_alloc_regions(world);

  for(x = 0; x < world->width; x++) {
    for(y = 0; y < world->height; y++) {
      for(z = 0; z < world->depth; z++) {
	if(!TILE_EQ(world->tiles[x][y][z], WALL) && world->regions[x][y][z] == 0) {
	  world->region_count++;
	  World_fill_region(world, world->region_count, x, y, z);
	}
      }
    }
  }
}

World *World_create()
{
  World *world = malloc(sizeof(World));
  int i = 0, j = 0, k = 0;
  Creature *creature;

  int height = WORLD_HEIGHT;
  int width = WORLD_WIDTH;
  int depth = WORLD_DEPTH;

  world->tiles = Tile_create(width, height, depth);

  for(i = 0; i < width; i++) {
    for(j = 0; j < height; j++) {
      for(k = 0; k < depth; k++) {
	world->tiles[i][j][k] = rand() % 2 == 1 ? WALL : FLOOR;
      }
    }
  }

  world->width = width;
  world->height = height;
  world->depth = depth;
  world->screen_height = SCREEN_HEIGHT;
  world->screen_width = SCREEN_WIDTH;
  world->tiles = Tile_smooth(world->tiles, 8, width, height, depth);
  world->region_count = 0;
  World_create_regions(world);
  World_connect_regions(world);

  world->screen_top = 0;
  world->screen_left = 0;

  world->creatures = List_create();
  world->messages = List_create();

  world->player = Creature_player_create(world);
  World_add_at_empty_location(world, world->player);

  for(i = 0; i < 50; i++) {
    creature = Creature_fungus_create(world);
    World_add_creature(world, creature);
    World_add_at_empty_location(world, creature);

  }

  return world;
}

void World_add_creature(World *world, Creature *creature)
{
  List_push(world->creatures, creature);
}

void World_remove_creature(World *world, Creature *creature)
{
  ListNode *node;

  LIST_FOREACH(world->creatures, first, next, cur) {
    if(cur->value == creature) {
      node = cur;
      break;
    }
  }

  die(node, "could not find the creature in the world.");

  Creature_destroy(node->value);
  List_remove(world->creatures, node);
}

void World_tick(World *world)
{
  Creature *creature;
  LIST_FOREACH(world->creatures, first, next, node) {
    creature = node->value;
    creature->ai->tick(creature->ai);
  }

  world->player->ai->tick(world->player->ai);
}

void World_destroy(World *world)
{
  Tile_destroy(world->tiles, world->height, world->width);
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

void World_dig(World *world, int x, int y, int z)
{
  if(Tile_is_diggable(World_tile(world, x, y, z))) {
    world->tiles[x][y][z] = FLOOR;
  }
}

int World_can_enter(World *world, int x, int y, int z)
{
  return Tile_is_ground(World_tile(world, x, y, z)) && World_creature_at(world, x, y, z) == NULL;
}

void World_add_at_empty_location(World *world, Creature *creature)
{
  int x = 0, y = 0, z = 0;

  do {
    x = rand() % WORLD_WIDTH;
    y = rand() % WORLD_HEIGHT;
    z = rand() % WORLD_DEPTH;
  } while(!World_can_enter(world, x, y, z));

  creature->x = x;
  creature->y = y;
  creature->z = z;
}

Creature *World_creature_at(World *world, int x, int y, int z)
{
  Creature *creature = NULL;

  LIST_FOREACH(world->creatures, first, next, node) {
    creature = node->value;
    if(creature->x == x && creature->y == y && creature->z == z) {
      return creature;
    }
  }

  if(world->player && world->player->x == x && world->player->y == y && world->player->z == z) {
    return world->player;
  }

  return NULL;
}

void World_notify(World *world, char *message)
{
  Message *m =  malloc(sizeof(Message));
  m->msg = message;
  m->life = 200;

  List_push(world->messages, m);
}
