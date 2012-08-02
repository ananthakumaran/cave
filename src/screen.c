#include <stdlib.h>
#include <curses.h>
#include "screen.h"
#include "dbg.h"
#include "utils.h"
#include "tile.h"
#include "creature.h"
#include "creature_ai.h"
#include "list.h"

#define ESCAPE_KEY 27
#define NEW_LINE_KEY 10
#define RETURN_KEY 13


void Screen_destroy(Screen *screen)
{
  World_destroy(screen->world);
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
      free(m);
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

  return Bitmap_isset(screen->visible, (X * Y * z) + (X * y) + x);
}

static void set_visible(Screen *screen, int x, int y, int z)
{

  int X = screen->world->width;
  int Y = screen->world->height;

  Bitmap_set(screen->visible, (X * Y * z) + (X * y) + x);
}


// start screen
void Startscreen_draw(Screen *screen)
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
      if(is_visible(screen, wx, wy, wz) || player->ai->can_see(player->ai, wx, wy, wz)) {
	set_visible(screen, wx, wy, wz);
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
      if(is_visible(screen, creature->x, creature->y, creature->z) || player->ai->can_see(player->ai, creature->x, creature->y, player->z)) {
	set_visible(screen, creature->x, creature->y, creature->z);
	Creature_draw(creature);
      }
    }
  }

  int status_board_height = world->screen_height + 1;
  move(status_board_height, 0);
  char status[50];
  sprintf(status, "Health %3d", world->player->hit_point);
  addstr(status);

  display_messages(world->messages, status_board_height + 1);
}


Screen* Startscreen_handle_input(Screen *screen, int key)
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
  }
  return screen;
}

void Startscreen_tick(Screen *screen)
{
  World_tick(screen->world);
}

Screen* Startscreen_create()
{
  Screen *screen = malloc(sizeof(Screen));
  die(screen, "Could not create screen.");

  screen->draw = Startscreen_draw;
  screen->handle_input = Startscreen_handle_input;
  screen->tick = Startscreen_tick;
  screen->world = World_create();
  screen->visible = Bitmap_create(screen->world->width * screen->world->height * screen->world->depth);

  die(screen->visible, "Out of memory");


  return screen;
}
