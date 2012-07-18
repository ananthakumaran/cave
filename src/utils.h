#ifndef _utils_h
#define _utils_h

int centeraddstr(int y, const char *str);

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define WITHIN(x, a, b) (((x) >= (a)) && ((x) <= (b)))

#endif
