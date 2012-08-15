#include <stdlib.h>
#include "world.h"
#include "creature.h"
#include "creature_ai.h"
#include "item.h"
#include "tile.h"
#include "dbg.h"
#include "utils.h"
#include "list.h"
#include "bitmap.h"

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
  int x, y, connected;

  List *connections = List_create();
  char *region_link = NULL;

  int r1, r2, rc, stairs, candidates_size;

  for(x = 0; x < world->width; x++) {
    for(y = 0; y < world->height; y++) {

      r1 = world->regions[x][y][z];
      r2 = world->regions[x][y][z + 1];

      rc = asprintf(&region_link, "%d - %d", r1, r2);
      die(rc != -1, "Out of memory");

      connected = 0;
      LIST_FOREACH(connections, first, next, cur) {
        if(strcmp(cur->value, region_link) == 0) {
          connected = 1;
          break;
        }
      }

      if(TILE_EQ(world->tiles[x][y][z], FLOOR) &&
         TILE_EQ(world->tiles[x][y][z + 1], FLOOR) &&
         (!connected)) {

        List_push(connections, region_link);

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
      } else {
        free(region_link);
      }
    }
  }

  List_clear_destroy(connections);
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


static void World_add_items(World *world)
{
  Item *item;
  int i = 0;
  Point *empty_location;

  for(i = 0; i < 500; i++) {
    empty_location = World_get_empty_location(world);
    item = Item_create_rock(empty_location);
    World_add_item(world, item);
  }
}

typedef Creature *(*Creature_create_fn)(World *world);

static void World_add_creatures(World *world, Creature_create_fn creator, int count)
{
  Creature *creature;

  int i = 0;
  for(i = 0; i < count; i++) {
    creature = creator(world);
    World_add_creature(world, creature);
    World_add_at_empty_location(world, creature);
  }
}

World *World_create()
{
  World *world = malloc(sizeof(World));
  int i = 0, j = 0, k = 0;

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
  world->dead_creatures = List_create();
  world->items = List_create();
  world->messages = List_create();

  world->player = Creature_player_create(world);
  World_add_at_empty_location(world, world->player);

  World_add_creatures(world, Creature_fungus_create, 500);
  World_add_creatures(world, Creature_apple_tree_create, 300);

  World_add_items(world);

  return world;
}

void World_add_creature(World *world, Creature *creature)
{
  List_push(world->creatures, creature);
}

void World_remove_creature(World *world, Creature *creature)
{
  int rc;

  rc = List_delete(world->creatures, creature);
  die(rc != 0, "could not find the creature in the world.");

  List_push(world->dead_creatures, creature);
  Creature_destroy(creature);
}

void World_add_item(World *world, Item *item)
{
  List_push(world->items, item);
}

void World_remove_item(World *world, Item *item)
{
  List_delete(world->items, item);
  Item_destroy(item);
}

void World_tick(World *world)
{
  Creature *creature;

  List* creatures_copy = List_copy(world->creatures);

  LIST_FOREACH(creatures_copy, first, next, node) {
    creature = node->value;

    if(!List_contains(world->dead_creatures, creature)) {
      creature->ai->tick(creature->ai);
    }
  }

  List_destroy(creatures_copy);
  List_destroy(world->dead_creatures);
  world->dead_creatures = List_create();

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
  return Tile_is_ground(World_tile(world, x, y, z)) && World_creature_at(world, x, y, z) == NULL &&
    World_item_at(world, x, y, z) == NULL;
}

int World_is_in_view(World *world, int x, int y)
{
  return WITHIN(x, world->screen_left, world->screen_left + world->screen_width) &&
    WITHIN(y, world->screen_top, world->screen_top + world->screen_height);
}

Point* World_get_empty_location(World *world)
{
  int x = 0, y = 0, z = 0;
  Point *point;

  do {
    x = rand() % WORLD_WIDTH;
    y = rand() % WORLD_HEIGHT;
    z = rand() % WORLD_DEPTH;
  } while(!World_can_enter(world, x, y, z));

  point = malloc(sizeof(Point));
  die(point, "Out of memory");

  point->x = x;
  point->y = y;
  point->z = z;

  return point;
}

void World_add_at_empty_location(World *world, Creature *creature)
{
  Point *empty_location = World_get_empty_location(world);

  creature->x = empty_location->x;
  creature->y = empty_location->y;
  creature->z = empty_location->z;

  free(empty_location);
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

Item *World_item_at(World *world, int x, int y, int z)
{
  Item *item = NULL;

  LIST_FOREACH(world->items, first, next, node) {
    item = node->value;
    if(item->owner == NULL && item->point->x == x &&
       item->point->y == y && item->point->z == z) {
      return item;
    }
  }

  return NULL;
}

void World_notify(World *world, char *message, int free_msg)
{
  Message *m =  malloc(sizeof(Message));
  m->msg = message;
  m->life = 200;
  m->free_msg = free_msg;

  List_push(world->messages, m);
}
