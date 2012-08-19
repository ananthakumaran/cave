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

List *Line(int x0, int y0, int x1, int y1)
{
  List *points = List_create();

  Point *point;

  int sx, sy, err, e2;
  int dx = abs(x1 - x0);
  int dy = abs(y1 - y0);

  sx = x0 < x1 ? 1 : -1;
  sy = y0 < y1 ? 1 : -1;

  err = dx - dy;

  while(1) {
    if(x0 == x1 && y0 == y1) break;

    point = Point_create(x0, y0, 0);
    List_push(points, point);

    e2 = 2 * err;

    if(e2 > -dy) {
      err -= dy;
      x0 += sx;
    }

    if(e2 < dx) {
      err += dx;
      y0 += sy;
    }
  }

  return points;
}
