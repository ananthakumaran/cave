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

static void print_glyphs()
{
  int i = 0;
  char num[50];
  int h = 0;
  int w = 0;
  for(i = 0; i < 127; i++) {

    w++;
    if(i % 10 == 0) {
      h++;
      w = 0;
      move(h, w);
    }

    addch(NCURSES_ACS(i));
    sprintf(num, " %d", i);
    addstr(num);
  }

  refresh();
  sleep(50);
}

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
  init_pair(6, COLOR_YELLOW, COLOR_BLACK);
  init_pair(7, COLOR_WHITE, COLOR_BLACK);
  init_pair(8, COLOR_BLACK, COLOR_WHITE);

  screen = Playscreen_create();
  game_loop();

  /* print_glyphs(); */

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

    usleep(5000);
    if(screen->tick) screen->tick(screen);

  } while(screen != NULL);

  finish(0);
}
