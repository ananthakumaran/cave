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
