#include <stdlib.h>
#include <curses.h>
#include "screen.h"
#include "dbg.h"
#include "utils.h"
#include "tile.h"
#include "creature.h"
#include "creature_ai.h"
#include "inventory.h"
#include "list.h"
#include "item.h"

#define ESCAPE_KEY 27
#define NEW_LINE_KEY 10
#define RETURN_KEY 13


void Screen_destroy(Screen *screen)
{
  free(screen);
}


static void remove_dead_messages(List *messages, List *dead_messages)
{
  LIST_FOREACH(dead_messages, first, next, cur) {
    List_remove(messages, cur->value);
  }
}

#define MAX_MESSAGES 3

static void display_messages(List *messages, int height)
{

  int space_left = MAX_MESSAGES;
  Message *m;

  List *dead_messages = List_create();

  LIST_FOREACH(messages, first, next, cur) {
    m = cur->value;

    if(m->life == 0) continue;

    move(height++, 0);
    clrtoeol();
    addstr(m->msg);

    space_left--;
    if(space_left == 0) break;

    m->life--;

    if(m->life < 1) {
      Message_destroy(m);
      List_push(dead_messages, cur);
    }
  }

  while(space_left--) {
    move(height++, 0);
    clrtoeol();
  }

  remove_dead_messages(messages, dead_messages);
  List_destroy(dead_messages);
}

static int is_visible(Screen *screen, int x, int y, int z)
{
  int X = screen->world->width;
  int Y = screen->world->height;
  int rc = Bitmap_isset(screen->visible, (X * Y * z) + (X * y) + x);
  die(rc != -1, "Out of bounds");
  return rc;
}

static void set_visible(Screen *screen, int x, int y, int z)
{

  int X = screen->world->width;
  int Y = screen->world->height;
  int rc;

  rc = Bitmap_set(screen->visible, (X * Y * z) + (X * y) + x);
  die(rc != 0, "Out of bounds");
}

static int can_see(Screen *screen, Creature *player, int x, int y, int z)
{
  if(is_visible(screen, x, y, z) ||
     player->ai->can_see(player->ai, x, y, z)) {
    set_visible(screen, x, y, z);
    return 1;
  }

  return 0;
}

static void display_items(Screen *screen)
{
  World *world = screen->world;
  Creature *player = world->player;
  Item *item;

  LIST_FOREACH(world->items, first, next, node) {
    item = node->value;

    if(player->z == item->point->z && item->owner == NULL &&
       can_see(screen, player, item->point->x, item->point->y, item->point->z) &&
       World_is_in_view(world, item->point->x, item->point->y)) {

      Tile_draw(item->tile, item->point->x - world->screen_left, item->point->y - world->screen_top);
    }
  }
}

static Screen *Screen_create(Screen_draw draw, Screen_handle_input handle_input)
{
  Screen *screen = malloc(sizeof(Screen));
  die(screen, "Could not create screen");

  screen->draw = draw;
  screen->handle_input = handle_input;

  return screen;
}


// play screen
static void Playscreen_draw(Screen *screen)
{
  int x = 0, y = 0, wx, wy, wz;
  Creature *creature;

  World *world = screen->world;
  Creature *player = world->player;

  World_center_by(world, player->x, player->y);

  for(y = 0; y <= world->screen_height; y++) {
    for(x = 0; x <= world->screen_width; x++) {

      wx = world->screen_left + x;
      wy = world->screen_top + y;
      wz = player->z;
      if(can_see(screen, player, wx, wy, wz)) {
	Tile tile = World_tile(world, wx, wy, wz);
	Tile_draw(tile, x, y);
      } else {
	Tile_draw(UNKNOWN, x, y);
      }
    }
  }

  Creature_draw(screen->world->player);

  LIST_FOREACH(screen->world->creatures, first, next, node) {
    creature = node->value;

    if(creature->z == player->z) {
      if(can_see(screen, player, creature->x, creature->y, creature->z)) {
	Creature_draw(creature);
      }
    }
  }

  display_items(screen);

  int status_board_height = world->screen_height + 1;
  move(status_board_height, 0);
  char status[50];
  clrtoeol();
  sprintf(status, "Health %3d, Power %d, Armor %s, Weapon %s",
	  player->hit_point,
	  CreatureAi_player_attack_value(player),
	  player->armor == NULL ? "none" : player->armor->name,
	  player->weapon == NULL ? "none" : player->weapon->name);
  addstr(status);

  display_messages(world->messages, status_board_height + 1);
}


static Screen* Playscreen_handle_input(Screen *screen, int key)
{
  Creature *player = screen->world->player;

  switch(key) {
  case KEY_UP:
  case 'k':
    Creature_move_by(player, 0, -1, 0);
    break;
  case KEY_DOWN:
  case 'j':
    Creature_move_by(player, 0, 1, 0);
    break;
  case KEY_LEFT:
  case 'h':
    Creature_move_by(player, -1, 0, 0);
    break;
  case KEY_RIGHT:
  case 'l':
    Creature_move_by(player, 1, 0, 0);
    break;
  case '<':
    Creature_move_by(player, 0, 0, -1);
    break;
  case '>':
    Creature_move_by(player, 0, 0, 1);
    break;

  case 'g':
  case ',':
    player->ai->pickup(player->ai, player->x, player->y, player->z);
    break;

  case 'd':
    screen = Inventoryscreen_create(screen, "drop");
    break;

  case 'w':
    screen = Inventoryscreen_create(screen, "wear");
    break;

  case 'e':
    CreatureAi_player_eat(player->ai);
    break;
  }

  return screen;
}

static void Playscreen_destroy(Screen *screen)
{
  World_destroy(screen->world);
  Bitmap_destroy(screen->visible);
  Screen_destroy(screen);
}

static Screen *Playscreen_tick(Screen *screen)
{
  World_tick(screen->world);

  if(!screen->world->player->alive) {
    Playscreen_destroy(screen);
    screen = Startscreen_create();
  }

  return screen;
}

Screen* Playscreen_create()
{
  Screen *screen = Screen_create(Playscreen_draw, Playscreen_handle_input);

  screen->tick = Playscreen_tick;
  screen->world = World_create();
  screen->visible = Bitmap_create(screen->world->width * screen->world->height * screen->world->depth);

  die(screen->visible, "Out of memory");


  return screen;
}

static Screen* Inventoryscreen_handle_input(Screen *screen, int key)
{
  World *world = screen->world;
  Creature *player = world->player;
  Screen *next_screen = screen;
  char *action = screen->action;
  Item *item;

  int index = key - 'a';

  if(WITHIN(index, 0, player->inventory_size - 1)) {
    item = player->inventory[index];

    if(item) {

      if(strcmp("drop", action) == 0) {
	player->ai->drop(player->ai, item);
      } else if(strcmp("wear", action) == 0) {
	player->ai->equip(player->ai, item);
      } else {
	die(0, "Unknown action in inventory screen");
      }

      next_screen = screen->parent;
      Screen_destroy(screen);
    } else {
      World_notify(world, "Empty slot", 0);
    }

  } else if(key == 'x') {
    next_screen = screen->parent;
    Screen_destroy(screen);
  } else {
    World_notify(world, "Unknown key", 0);
  }

  return next_screen;
}


static void Inventoryscreen_draw(Screen *screen)
{
  World *world = screen->world;
  Creature *player = world->player;

  char c = 'a';
  int i = 0;


  for(i = 0; i < player->inventory_size; i++, c++) {
    move(i, 0);
    clrtoeol();

    addch(c);
    addch(' ');

    if(player->inventory[i] == NULL) {
      addstr("empty");
    } else {
      addstr(player->inventory[i]->name);
    }
  }

  move(i, 0);
  clrtoeol();
  addstr("What would you like to ");
  addstr(screen->action);
  addstr(" ?");

  display_messages(world->messages, world->screen_height  + 1);

}

Screen* Inventoryscreen_create(Screen *play_screen, char *action)
{
  Screen *screen = Screen_create(Inventoryscreen_draw, Inventoryscreen_handle_input);

  screen->tick = NULL;
  screen->world = play_screen->world;
  screen->parent = play_screen;
  screen->action = action;


  return screen;
}

// start screen
static Screen* Startscreen_handle_input(Screen *screen, int key)
{
  switch(key) {
  case 's':
    Screen_destroy(screen);
    screen = Playscreen_create();
    break;
  case 'q':
    Screen_destroy(screen);
    screen = NULL;
    break;
  }

  return screen;
}

static void Startscreen_draw(Screen *screen)
{

  int height = 10;
  size_t i;
  char *banner[] = {
    "Welcome to cave",
    "",
    "",
    "Press [s] to start or [q] to quit",
    "",
    "-----------",
    "",
    ""
  };

  for(i = 0; i < (sizeof(banner) / sizeof(char *)); i++) {
    move(height + i, 0);
    addstr(banner[i]);
  }

}


Screen* Startscreen_create()
{
  Screen *screen = Screen_create(Startscreen_draw, Startscreen_handle_input);
  screen->tick = NULL;

  return screen;
}
