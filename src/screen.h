#ifndef _screen_
#define _screen_h

#include "world.h"

typedef struct Screen Screen;

typedef void (*Screen_draw)(Screen *screen);
typedef Screen* (*Screen_handle_input)(Screen *screen, int key);
typedef void (*Screen_tick)(Screen *screen);

struct Screen {
  Screen_draw draw;
  Screen_handle_input handle_input;
  Screen_tick tick;
  World *world;
};


void Screen_destroy(Screen *screen);

// startscreen
void Startscreen_draw();
Screen* Startscreen_handle_input();
Screen* Startscreen_create();

#endif
