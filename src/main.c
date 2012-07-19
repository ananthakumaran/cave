#include <curses.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "dbg.h"
#include "utils.h"
#include "screen.h"


static void finish(int sig);
static void game_loop();

static Screen *screen = NULL;

int main()
{

  signal(SIGINT, finish);
  srand(time(NULL));

  initscr();
  curs_set(0); // hide the cursor
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  set_escdelay(10);
  cbreak(); // return after reading one char
  noecho();

  start_color();

  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  init_pair(2, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(3, COLOR_RED, COLOR_BLACK);
  init_pair(4, COLOR_BLUE, COLOR_BLACK);
  init_pair(5, COLOR_CYAN, COLOR_BLACK);

  screen = Startscreen_create();
  game_loop();
  finish(0);
}

static void finish(int sig)
{
  endwin();
  exit(sig);
}

void game_loop()
{
  int input;

  do {

    screen->draw(screen);
    refresh();

    input = getch();
    if(input != ERR) {
      screen = screen->handle_input(screen, input);
    }

    usleep(500);
    if(screen->tick) screen->tick(screen);

  } while(screen != NULL);

  finish(0);
}
