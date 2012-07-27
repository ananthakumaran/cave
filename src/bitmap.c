#include <bitmap.h>
#include <dbg.h>


Bitmap *Bitmap_create(size_t size)
{
  check(size > 0, "size should be greater than zero.");

  int byte_required = ((size - 1) / 8) + 1;

  Bitmap *map = calloc(1, sizeof(Bitmap) + byte_required);
  map->size = size;

  return map;
 error:
  return NULL;
}

static char *Bitmap_byte(Bitmap *map, size_t n)
{
  return map->data + ((n + 1) / 8 + 1);
}

int Bitmap_set(Bitmap *map, size_t n)
{
  check(n >= 0 && n < map->size, "out of bounds.");

  int offset = n % 8;
  char *byte = Bitmap_byte(map, n);

  *byte |= (1 << offset);

  return 1;
 error:
  return 0;
}

int Bitmap_unset(Bitmap *map, size_t n)
{
  check(n >= 0 && n < map->size, "out of bounds.");

  int offset = n % 8;
  char *byte = Bitmap_byte(map, n);

  *byte &= ~(1 << offset);

  return 1;
 error:
  return 0;
}

int Bitmap_isset(Bitmap *map, size_t n)
{
  check(n >= 0 && n < map->size, "out of bounds.");

  int offset = n % 8;
  char *byte = Bitmap_byte(map, n);

  return *byte & (1 << offset);

 error:
  return -1;
}

void Bitmap_destroy(Bitmap *map)
{
  free(map);
}
