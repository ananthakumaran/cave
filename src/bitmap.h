#ifndef _bitmap_h
#define _bitmap_h

#include <stdlib.h>

typedef struct Bitmap {
  size_t size;
  char data[];
} Bitmap;

Bitmap *Bitmap_create(size_t size);
int Bitmap_set(Bitmap *map, size_t n);
int Bitmap_unset(Bitmap *map, size_t n);
int Bitmap_isset(Bitmap *map, size_t n);
void Bitmap_destroy(Bitmap *map);

#endif
