#include <stdlib.h>
#include "creature_ai.h"
#include "creature.h"
#include "tile.h"
#include "inventory.h"
#include "utils.h"
#include "dbg.h"

static void CreatureAi_hit_default(CreatureAi *ai, int power)
{
  ai->creature->hit_point -= power;
  if(ai->creature->hit_point < 1) {
    World_remove_creature(ai->creature->world, ai->creature);
  }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static void CreatureAi_enter_default(CreatureAi *ai, int x, int y, int z)
{
// noop
}

static void CreatureAi_tick_default(CreatureAi *ai)
{
// noop
}

static int CreatureAi_can_see_default(CreatureAi *ai, int x, int y, int z)
{
  return 0;
}

#pragma GCC diagnostic pop


static CreatureAi *CreatureAi_create(Creature *creature, CreatureAi_enter enter, CreatureAi_tick tick, CreatureAi_hit hit, CreatureAi_can_see can_see)
{
  CreatureAi *ai = malloc(sizeof(CreatureAi));
  ai->creature = creature;

  if(tick) {
    ai->tick = tick;
  } else {
    ai->tick = CreatureAi_tick_default;
  }

  if(enter) {
    ai->enter = enter;
  } else {
    ai->enter = CreatureAi_enter_default;
  }

  if(hit) {
    ai->hit = hit;
  } else {
    ai->hit = CreatureAi_hit_default;
  }

  if(can_see) {
    ai->can_see = can_see;
  } else {
    ai->can_see = CreatureAi_can_see_default;
  }

  return ai;
}

void CreatureAi_destroy(CreatureAi *ai)
{
  free(ai);
}


static void CreatureAi_player_attack(Creature *player, Creature *creature)
{
  int amount = MAX(1, player->attack_value - creature->defense_value);
  amount = rand() % amount;

  char *message = NULL;
  asprintf(&message, "You have attacked a %s", creature->name);
  World_notify(player->world, message, 1);

  creature->ai->hit(creature->ai, amount);
}

static void CreatureAi_player_enter(CreatureAi *ai, int x, int y, int z)
{
  Tile tile = World_tile(ai->creature->world, x, y, z);
  Creature *player = ai->creature;

  int dz = z - player->z;

  Tile current_tile = World_tile(player->world, player->x, player->y, player->z);

  if(dz == -1) {
    if(TILE_EQ(current_tile, STAIR_UP)) {
      World_notify(player->world, "Walk up the stairs", 0);
    } else {
      World_notify(player->world, "Not stairs in the top", 0);
      return;
    }
  } else if(dz == 1) {
    if(TILE_EQ(current_tile, STAIR_DOWN)) {
      World_notify(player->world, "Walk down the stairs", 0);
    } else {
      World_notify(player->world, "No stairs in the bottom", 0);
      return;
    }
  }

  if(dz != 0) {
    player->x = x;
    player->y = y;
    player->z = z;
    return;
  }

  Creature *other = World_creature_at(player->world, x, y, z);

  if(other) {
    CreatureAi_player_attack(player, other);
  } else {
    if(Tile_is_ground(tile)) {
      player->x = x;
      player->y = y;
    } else if(Tile_is_diggable(tile) && player->hit_point > 10) {

      player->hit_point -= 10;
      World_dig(player->world, x, y, z);
      player->x = x;
      player->y = y;

    }
  }
}


static List *Line(int x0, int y0, int x1, int y1)
{
  List *points = List_create();

  Point *point;

  int sx, sy, err, e2;
  int dx = abs(x1 - x0);
  int dy = abs(y1 - y0);

  sx = x0 < x1 ? 1 : -1;
  sy = y0 < y1 ? 1 : -1;

  err = dx - dy;

  while(1) {
    if(x0 == x1 && y0 == y1) break;

    point = Point_create(x0, y0, 0);
    List_push(points, point);

    e2 = 2 * err;

    if(e2 > -dy) {
      err -= dy;
      x0 += sx;
    }

    if(e2 < dx) {
      err += dx;
      y0 += sy;
    }
  }

  return points;
}

static int CreatureAi_player_can_see(CreatureAi *ai, int x, int y, int z)
{
  Creature *player = ai->creature;

  if(player->z != z) return 0;

  if((player->x - x) * (player->x - x) + (player->y - y) * (player->y - y) >
     player->vision_radius * player->vision_radius) return 0;

  List *points = Line(player->x, player->y, x, y);

  Point *p;
  LIST_FOREACH(points, first, next, cur) {
    p = cur->value;
    if(!(Tile_is_ground(World_tile(player->world, p->x, p->y, player->z)) || (player->x == p->x && player->y == p->y))) {
      List_clear_destroy(points);
      return 0;
    }
  }

  List_clear_destroy(points);
  return 1;
}

static void CreatureAi_player_pickup(CreatureAi *ai, int x, int y, int z)
{
  Creature *player = ai->creature;
  World *world = player->world;
  Item *item = World_item_at(world, x, y, z);
  char *message = NULL;

  if(item == NULL) {
    World_notify(world, "Grab at the ground", 0);
  } else if(Inventory_is_full(player)) {
    World_notify(world, "Inventory is full", 0);
  } else {
    asprintf(&message, "Pickup a %s", item->name);
    World_notify(world, message, 1);
    Inventory_add(player, item);
  }
}

void CreatureAi_player_eat(CreatureAi *ai)
{
  Creature *player = ai->creature;
  World *world = player->world;
  Item *item = World_item_at(world, player->x, player->y, player->z);

  if(item == NULL) {
    World_notify(world, "Nothing to eat", 0);
  } else {
    player->hit_point += 10;
    World_notify(world, "Eat a apple", 0);
    World_remove_item(world, item);
  }
}

static void CreatureAi_player_drop(CreatureAi *ai, Item *item)
{
  Creature *player = ai->creature;
  World *world = player->world;
  char *message = NULL;
  asprintf(&message, "Drop a %s", item->name);
  World_notify(world, message, 1);
  Inventory_remove(player, item);

  Item_set_point(item, World_get_empty_location(world));
}

CreatureAi *CreatureAi_player_create(Creature *player)
{
  CreatureAi *player_ai =  CreatureAi_create(player, CreatureAi_player_enter, NULL, NULL, CreatureAi_player_can_see);
  player_ai->pickup = CreatureAi_player_pickup;
  player_ai->drop = CreatureAi_player_drop;
  return player_ai;
}

#define FUNGI_SPREAD_DISTANCE 10

static void fungus_spread(CreatureAi *fungus_ai)
{
  Creature *child;
  World *world = fungus_ai->creature->world;
  int x = fungus_ai->creature->x + (rand() % FUNGI_SPREAD_DISTANCE) - FUNGI_SPREAD_DISTANCE / 2;
  int y = fungus_ai->creature->y + (rand() % FUNGI_SPREAD_DISTANCE) - FUNGI_SPREAD_DISTANCE / 2;
  int z = fungus_ai->creature->z + ((rand() % 3) - 1);

  if(World_can_enter(world, x, y, z)) {
    child = Creature_fungus_create(world);
    World_add_creature(world, child);
    child->x = x;
    child->y = y;
    child->z = z;
    fungus_ai->spread_count++;
  }
}

static void CreatureAi_fungus_enter(CreatureAi *ai, int x, int y, int z)
{
  Creature *fungus = ai->creature;
  Tile tile = World_tile(fungus->world, x, y, z);

  Creature *other = World_creature_at(ai->creature->world, x, y, z);

  if(other) {
    other->ai->hit(other->ai, fungus->attack_value);
  } else if(Tile_is_ground(tile)) {
     fungus->x = x;
     fungus->y = y;
     fungus->z = z;
   }
}

static void CreatureAi_fungus_tick(CreatureAi *fungus_ai)
{
  Creature *fungus = fungus_ai->creature;

  if(fungus_ai->spread_count < 5 && rand() % 5000 == 0) {
    fungus_spread(fungus_ai);
  }

  // wander
  if(rand() % 100 == 0) {
    int mx = (rand() % 3) - 1;
    int my = (rand() % 3) - 1;

    Creature_move_by(fungus, mx, my, 0);
  }
}

CreatureAi *CreatureAi_fungus_create(Creature *fungus)
{
  CreatureAi *fungus_ai = CreatureAi_create(fungus, CreatureAi_fungus_enter, CreatureAi_fungus_tick, NULL, NULL);
  fungus_ai->spread_count = 0;
  return fungus_ai;
}

// apple tree

static void CreatureAi_apple_tree_tick(CreatureAi *apple_tree_ai)
{
  Creature *apple_tree = apple_tree_ai->creature;
  World *world = apple_tree->world;
  int x, y, z;
  Point *point;
  Item *item;

  if(rand() % 10000 == 0) {
    x = (rand() % 10) - 5 + apple_tree->x;
    y = (rand() % 10) - 5 + apple_tree->y;
    z = apple_tree->z;

    if(World_can_enter(world, x, y, z)) {
      point = Point_create(x, y, z);
      item = Item_create_apple(point);
      World_add_item(world, item);
    }
  }
}

CreatureAi *CreatureAi_apple_tree_create(Creature *apple_tree)
{
  CreatureAi *apple_tree_ai = CreatureAi_create(apple_tree, NULL, CreatureAi_apple_tree_tick, NULL, NULL);
  return apple_tree_ai;
}
