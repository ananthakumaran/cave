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
extern Tile STAIR_UP;
extern Tile STAIR_DOWN;

Tile ***Tile_create(int width, int height, int depth);
void Tile_destroy(Tile ***tiles, int height, int width);
Tile ***Tile_smooth(Tile ***tiles, int times, int width, int height, int depth);

#define TILE_EQ(A, B) ((A).glyph == (B).glyph && (A).color == (B).color)

int Tile_is_diggable(Tile tile);
int Tile_is_ground(Tile tile);

void Tile_draw(Tile tile, int x, int y);

#endif
