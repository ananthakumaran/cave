#ifndef __tile_h_
#define __tile_h_

#include <curses.h>

struct World;

typedef struct Tile {
  char glyph;
  int color;
} Tile;

extern Tile FLOOR;
extern Tile WALL;
extern Tile BOUNDS;
extern Tile STAIR_UP;
extern Tile STAIR_DOWN;
extern Tile UNKNOWN;
extern Tile ROCK;
extern Tile APPLE;
extern Tile DAGGER;
extern Tile SWORD;
extern Tile STAFF;
extern Tile TUNIC;
extern Tile CHAIN_MAIL;
extern Tile PLATE_MAIL;

Tile ***Tile_create(int width, int height, int depth);
void Tile_destroy(Tile ***tiles, int height, int width);
Tile ***Tile_smooth(Tile ***tiles, int times, int width, int height, int depth);

#define TILE_EQ(A, B) ((A).glyph == (B).glyph && (A).color == (B).color)

int Tile_is_diggable(Tile tile);
int Tile_is_ground(Tile tile);

void Tile_draw(Tile tile, int x, int y);

#endif
