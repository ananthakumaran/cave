#ifndef __tile_h_
#define __tile_h_

#include <curses.h>

typedef struct Tile {
  char glyph;
  int color;
} Tile;

extern Tile FLOOR;
extern Tile WALL;
extern Tile BOUNDS;

Tile **Tile_create(int width, int height);
void Tile_destroy(Tile **tiles, int height);
Tile **Tile_smooth(Tile **tiles, int times, int width, int height);

#define TILE_EQ(A, B) ((A).glyph == (B).glyph && (A).color == (B).color)

#endif
