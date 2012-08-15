#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include "dbg.h"
#include "tile.h"
#include "world.h"
#include "utils.h"

Tile BOUNDS = {
  .glyph = 'n',
  .color = 3
};

Tile WALL = {
  .glyph = 'h',
  .color = 1
};

Tile FLOOR = {
  .glyph = '~',
  .color = 2
};

Tile STAIR_UP = {
  .glyph = '-',
  .color = 8
};

Tile STAIR_DOWN = {
  .glyph = '.',
  .color = 8
};

Tile UNKNOWN = {
  .glyph = ' ',
  .color = 5
};

Tile ROCK = {
  .glyph = 'h',
  .color = 3
};

Tile APPLE = {
  .glyph = 105,
  .color = 2
};




Tile ***Tile_create(int width, int height, int depth)
{
  int i, j;
  Tile ***tiles = malloc(width * sizeof(Tile *));
  die(tiles, "Out of memory");

  for(i = 0; i < width; i++) {
    tiles[i] = malloc(height * sizeof(Tile *));
    die(tiles[i], "Out of memory");

    for(j = 0; j < height; j++) {
      tiles[i][j] = malloc(depth * sizeof(Tile));
      die(tiles[i][j], "Out of memory");
    }
  }

  return tiles;
}

void Tile_destroy(Tile ***tiles, int height, int width)
{
  int i = 0, j = 0;
  for(i = 0; i < width; i++) {
    for(j = 0; j < height; j++) {
      free(tiles[i][j]);
    }
    free(tiles[i]);
  }

  free(tiles);
}

Tile ***Tile_smooth(Tile ***tiles, int times, int width, int height, int depth)
{
  check(times > 0, "times should be greater than 0.");

  int i = 0, j = 0, k = 0, ni = 0, nj = 0, floors = 0, walls = 0;
  Tile ***smoothed = Tile_create(width, height, depth), ***temp;

  while(times--) {
    for(i = 0; i < width; i++) {
      for(j = 0; j < height; j++) {
	for(k = 0; k < depth; k++) {
	  floors = 0;
	  walls = 0;

	  for(ni = i - 1; ni <= i + 1; ni++) {
	    if(ni < 0 || ni >= width) continue;
	    for(nj = j - 1; nj <= j + 1; nj++) {
	      if(nj < 0 || nj >= height) continue;

	      if(TILE_EQ(tiles[ni][nj][k], WALL)) {
		walls++;
	      } else {
		floors++;
	      }
	    }
	  }

	  smoothed[i][j][k] = floors > walls ? FLOOR : WALL;
	}
      }
    }

    temp = tiles;
    tiles = smoothed;
    smoothed = temp;
  }

  Tile_destroy(smoothed, height, width);
  return tiles;
 error:
  return NULL;
}

int Tile_is_diggable(Tile tile)
{
  return TILE_EQ(tile, WALL);
}

int Tile_is_ground(Tile tile)
{
  return !TILE_EQ(tile, WALL) && !TILE_EQ(tile, BOUNDS);
}

void Tile_draw(Tile tile, int x, int y)
{
  move(y, x);
  attrset(COLOR_PAIR(tile.color));
  addch(NCURSES_ACS(tile.glyph));
}
