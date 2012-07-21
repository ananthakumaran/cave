#include <stdlib.h>
#include <curses.h>
#include "screen.h"
#include "dbg.h"
#include "utils.h"
#include "tile.h"
#include "creature.h"
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

// start screen
void Startscreen_draw(Screen *screen)
{
  int x = 0, y = 0;
  Creature *creature;

  World *world = screen->world;

  World_center_by(world, world->player->x, world->player->y);

  for(y = 0; y <= world->screen_height; y++) {
    for(x = 0; x <= world->screen_width; x++) {

      Tile tile = World_tile(world, world->screen_left + x, world->screen_top + y);
      Tile_draw(tile, x, y);
    }
  }

  Creature_draw(screen->world->player);

  LIST_FOREACH(screen->world->creatures, first, next, node) {
    creature = node->value;

    Creature_draw(creature);
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
    Creature_move_by(player, 0, -1);
    break;
  case KEY_DOWN:
  case 'j':
    Creature_move_by(player, 0, 1);
    break;
  case KEY_LEFT:
  case 'h':
    Creature_move_by(player, -1, 0);
    break;
  case KEY_RIGHT:
  case 'l':
    Creature_move_by(player, 1, 0);
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

  return screen;
}
