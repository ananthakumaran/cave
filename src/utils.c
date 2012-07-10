#include "utils.h"
#include <curses.h>
#include <string.h>

int centeraddstr(int y, const char *str)
{
  move(y, (COLS / 2) - (strlen(str) / 2));
  return addstr(str);
}
