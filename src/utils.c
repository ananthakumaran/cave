#include "utils.h"
#include <curses.h>
#include <string.h>
#include <stdlib.h>

int centeraddstr(int y, const char *str)
{
  move(y, (COLS / 2) - (strlen(str) / 2));
  return addstr(str);
}

Point *Point_create(int x, int y, int z)
{
  Point *point = malloc(sizeof(Point));
  point->x = x;
  point->y = y;
  point->z = z;

  return point;
}
