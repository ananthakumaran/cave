#ifndef _path_h
#define _path_h

#include "list.h"
#include "world.h"
#include "utils.h"

List *Path(World *world, Point *source, Point *destination);

#endif
