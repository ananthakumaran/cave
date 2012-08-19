#ifndef _screen_
#define _screen_h

#include "world.h"
#include "bitmap.h"

typedef struct Screen Screen;

typedef void (*Screen_draw)(Screen *screen);
typedef Screen* (*Screen_handle_input)(Screen *screen, int key);
typedef void (*Screen_tick)(Screen *screen);

struct Screen {
  Screen_draw draw;
  Screen_handle_input handle_input;
  Screen_tick tick;
  World *world;
  Bitmap *visible;

  Screen* parent;
  char *action;
};


void Screen_destroy(Screen *screen);

// play screen
void Playscreen_draw(Screen *screen);
Screen* Playscreen_handle_input(Screen *screen, int key);
Screen* Playscreen_create();

// inventory screen
void Inventoryscreen_draw();
Screen* Inventoryscreen_handle_input(Screen *screen, int key);
Screen* Inventoryscreen_create(Screen *play_screen, char *action);

#endif
