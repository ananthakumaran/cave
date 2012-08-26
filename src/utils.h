#ifndef _utils_h
#define _utils_h

#include "list.h"

int centeraddstr(int y, const char *str);

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define WITHIN(x, a, b) (((x) >= (a)) && ((x) <= (b)))

typedef struct Point {
  int x;
  int y;
  int z;
} Point;

Point *Point_create(int x, int y, int z);
#define POINT_EQ(A, B) ((A)->x == (B)->x && (A)->y == (B)->y && (A)->z == (B)->z)

List *Line(int x0, int y0, int x1, int y1);



#endif
