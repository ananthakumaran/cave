#include <curses.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "dbg.h"
#include "utils.h"
#include "screen.h"


static int finish();
static void game_loop();

static Screen *screen = NULL;

int main()
{
  initscr();
  curs_set(0); // hide the cursor
  keypad(stdscr, TRUE);
  set_escdelay(10);
  cbreak(); // return after reading one char
  noecho();

  if(has_colors()) {
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
  }

  attrset(COLOR_PAIR(1));

  screen = Startscreen_create();
  game_loop();
  finish();
}

static int finish()
{
  endwin();
  exit(0);
}

void game_loop()
{
  int input;
  Screen *new_screen;

  do {
    clear();
    screen->draw();
    refresh();
    timeout(-1);
    input = getch();
    new_screen = screen->handle_input(input);
    free(screen);
    screen = new_screen;
  } while(screen != NULL);

  finish();
}
