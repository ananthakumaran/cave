#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include "dbg.h"
#include "tile.h"

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


Tile **Tile_create(int width, int height)
{
  int i;
  Tile **tiles = malloc(height * sizeof(Tile *));
  die(tiles, "Out of memory");

  for(i = 0; i < height; i++) {
    tiles[i] = malloc(width * sizeof(Tile));
    die(tiles[i], "Out of memory");
  }

  return tiles;
}

void Tile_destroy(Tile **tiles, int height)
{
  int i = 0;

  for(i = 0; i < height; i++) {
    free(tiles[i]);
  }

  free(tiles);
}

Tile **Tile_smooth(Tile **tiles, int times, int width, int height)
{
  check(times > 0, "times should be greater than 0.");

  int i = 0, j = 0, ni = 0, nj = 0, floors = 0, walls = 0;
  Tile **smoothed = Tile_create(width, height), **temp;

  while(times--) {
    for(i = 0; i < height; i++) {
      for(j = 0; j < width; j++) {

	floors = 0;
	walls = 0;

	for(ni = i - 1; ni <= i + 1; ni++) {
	  if(ni < 0 || ni >= height) continue;
	  for(nj = j - 1; nj <= j + 1; nj++) {
	    if(nj < 0 || nj >= width) continue;

	    if(TILE_EQ(tiles[ni][nj], WALL)) {
	      walls++;
	    } else {
	      floors++;
	    }
	  }
	}

	smoothed[i][j] = floors > walls ? FLOOR : WALL;
      }
    }

    temp = tiles;
    tiles = smoothed;
    smoothed = temp;
  }

  Tile_destroy(smoothed, height);
  return tiles;
 error:
  return NULL;
}
