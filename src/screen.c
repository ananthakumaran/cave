#include <stdlib.h>
#include <curses.h>
#include "screen.h"
#include "dbg.h"
#include "utils.h"
#include "tile.h"

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

  World *world = screen->world;

  for(y = 0; y < world->screen_height; y++) {
    for(x = 0; x < world->screen_width; x++) {
      move(y, x);

      Tile tile = World_tile(world, world->screen_left + x, world->screen_top + y);
      attrset(COLOR_PAIR(tile.color));
      addch(NCURSES_ACS(tile.glyph));
    }
  }
}

Screen* Startscreen_handle_input(Screen *screen, int key)
{
  switch(key) {
  case KEY_UP:
  case 'k':
    World_scroll_by(screen->world, 0, -2);
    break;
  case KEY_DOWN:
  case 'j':
    World_scroll_by(screen->world, 0, 2);
    break;
  case KEY_LEFT:
  case 'h':
    World_scroll_by(screen->world, -2, 0);
    break;
  case KEY_RIGHT:
  case 'l':
    World_scroll_by(screen->world, 2, 0);
    break;
  }
  return screen;
}

Screen* Startscreen_create()
{
  Screen *screen = malloc(sizeof(Screen));
  die(screen, "Could not create screen.");

  screen->draw = Startscreen_draw;
  screen->handle_input = Startscreen_handle_input;
  screen->world = World_create();
  return screen;
}
