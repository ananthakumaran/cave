#ifndef _screen_h
#define _screen_h

typedef struct Screen Screen;

typedef void (*Screen_draw)();
typedef Screen* (*Screen_handle_input)(int key);

struct Screen {
  Screen_draw draw;
  Screen_handle_input handle_input;
};

// startscreen
void Startscreen_draw();
Screen* Startscreen_handle_input();
Screen* Startscreen_create();

// playscreen
void Playscreen_draw();
Screen* Playscreen_handle_input();
Screen* Playscreen_create();

// winscreen
void Winscreen_draw();
Screen* Winscreen_handle_input();
Screen* Winscreen_create();

// losescreen
void Losescreen_draw();
Screen* Losescreen_handle_input();
Screen* Losescreen_create();

#endif
