#include "utils.h"
#include <curses.h>
#include <string.h>

int centeraddstr(const char *str)
{
  move(LINES/2, (COLS / 2) - (strlen(str) / 2));
  return addstr(str);
}
