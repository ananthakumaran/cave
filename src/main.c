#include <curses.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "utils.h"

int main()
{
    initscr();
    curs_set(0);

    if(has_colors()) {
      start_color();
      init_pair(1, COLOR_RED, COLOR_BLACK);
    }

    attrset(COLOR_PAIR(1));
    centeraddstr("Welcome to Cave");
    refresh();
    sleep(2);
    endwin();
    exit(0);
}
