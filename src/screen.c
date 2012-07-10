#include <stdlib.h>
#include <curses.h>
#include "screen.h"
#include "dbg.h"
#include "utils.h"

#define ESCAPE_KEY 27
#define NEW_LINE_KEY 10
#define RETURN_KEY 13


static Screen* Screen_create(Screen_draw draw, Screen_handle_input handle_input)
{
  Screen *screen = malloc(sizeof(Screen));
  die(screen, "Could not create screen.");

  screen->draw = draw;
  screen->handle_input = handle_input;
  return screen;
}

// start screen
void Startscreen_draw()
{
  centeraddstr(1, "Cave game.");
  centeraddstr(2, "-- press [enter] to start --");
}

Screen* Startscreen_handle_input(int key)
{
  switch(key) {
  case NEW_LINE_KEY:
  case RETURN_KEY:
    return Playscreen_create();
  default:
    return Startscreen_create();
  }
}

Screen* Startscreen_create()
{
  return Screen_create(Startscreen_draw, Startscreen_handle_input);
}

// playscreen

void Playscreen_draw()
{
  centeraddstr(1, "You are having fun.");
  centeraddstr(2, "-- press [escape] to loose or [enter] to win --");
}

Screen* Playscreen_handle_input(int key)
{
  switch(key) {
  case ESCAPE_KEY:
    return Losescreen_create();
  case NEW_LINE_KEY:
  case RETURN_KEY:
    return Winscreen_create();
  default:
    return Playscreen_create();
  }
}

Screen* Playscreen_create()
{
  return Screen_create(Playscreen_draw, Playscreen_handle_input);
}

// winscreen
void Winscreen_draw()
{
  centeraddstr(1, "You won.");
  centeraddstr(2, "-- press [enter] to restart or [escape] to exit --");
}

Screen* Winscreen_handle_input(int key)
{
  switch(key) {
  case ESCAPE_KEY:
    return NULL;
  case NEW_LINE_KEY:
  case RETURN_KEY:
    return Playscreen_create();
  default:
    return Winscreen_create();
  }
}

Screen* Winscreen_create()
{
  return Screen_create(Winscreen_draw, Winscreen_handle_input);
}

// losescreen
void Losescreen_draw()
{
  centeraddstr(1, "You lost.");
  centeraddstr(2, "-- press [enter] to restart or [escape] to exit --");
}

Screen* Losescreen_handle_input(int key)
{
  switch(key) {
  case ESCAPE_KEY:
    return NULL;
  case NEW_LINE_KEY:
  case RETURN_KEY:
    return Playscreen_create();
  default:
    return Losescreen_create();
  }
}

Screen* Losescreen_create()
{
  return Screen_create(Losescreen_draw, Losescreen_handle_input);
}
