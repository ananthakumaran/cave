#include <stdint.h>
#include <math.h>
#include "path.h"
#include "hashmap.h"
#include "p_queue.h"

typedef struct Node {
  int gscore;
  int fscore;
  Point *point;
  struct Node *came_from;
} Node;

static int cmp(void *a, void *b)
{
  Node *na = a;
  Node *nb = b;

  if(POINT_EQ(na->point, nb->point)) {
    return 0;
  } else {
    return 1;
  }
}

static uint32_t hash(void *p)
{
  Node *node = p;
  return node->point->x ^ node->point->y ^ node->point->z;
}

static int square(int x)
{
  return x * x;
}

static int heuristic_cost_estimate(Point *source, Point *destination)
{
  return sqrt(square(destination->x - source->x) +
	      square(destination->y - source->y));
}

static Node *Node_create(Point *point, int gscore, int fscore, Node *came_from) {
  Node *node = malloc(sizeof(Node));
  die(node, "Out of memory");

  node->point = point;
  node->fscore = fscore;
  node->gscore = gscore;
  node->came_from = came_from;

  return node;
}

static void Node_destroy(Node *node)
{
  if(node) {
    free(node->point);
    free(node);
  }
}

static List *reconstruct_path(Node *node)
{
  List *path = List_create();

  while(node) {
    if(node->came_from) {
      Point *p = Point_create(node->point->x, node->point->y, node->point->z);
      List_shift(path, p);
    }

    node = node->came_from;
  }

  return path;
};

static int traverse_clean(HashmapNode *hnode)
{
  Node *node = hnode->key;
  Node_destroy(node);
  return 0;
}

static List *neighbours_list(World *world, Point *point, Point *destination, Hashmap *nodes)
{
  List *neighbours = List_create();
  int nx, ny;

  for(nx = point->x - 1; nx <= point->x + 1; nx++) {
    if(nx < 0 || nx >= world->width) continue;
    for(ny = point->y - 1; ny <= point->y + 1; ny++) {
      if(ny < 0 || ny >= world->height ||
	 (ny == point->y && nx == point->x) ||
	 (!World_can_enter(world, nx, ny, point->z) &&
	  !(nx == destination->x && ny == destination->y))) continue;

      Point *p  = Point_create(nx, ny, point->z);
      Node *node = Node_create(p, 0, 0, NULL);

      Node *old_node = Hashmap_get(nodes, node);
      if(old_node) {
	Node_destroy(node);
	node = old_node;
      } else {
	Hashmap_set(nodes, node, node);
      }

      List_push(neighbours, node);
    }
  }

  return neighbours;
}

List *Path(World *world, Point *source, Point *destination)
{
  int tentative_gscore;
  List *result = NULL;
  int tries = 0;

  Hashmap *nodes = Hashmap_create(cmp, hash);
  Hashmap *closedset = Hashmap_create(cmp, hash);
  PQueue *openset = PQueue_create(cmp, hash);

  Node *current;
  Node *start = Node_create(source, 0, 0, NULL);
  Hashmap_set(nodes, start, start);

  start->fscore = start->gscore + heuristic_cost_estimate(start->point, destination);
  PQueue_push(openset, start, start->fscore);

  while(!PQueue_empty(openset) &&
	tries < 300) {
    tries++;


    current = PQueue_pop(openset);
    Hashmap_set(closedset, current, current);

    if(POINT_EQ(current->point, destination)) {
      result = reconstruct_path(current);
      break;

    } else {

      List *neighbours = neighbours_list(world, current->point, destination, nodes);

      LIST_FOREACH(neighbours, first, next, cur) {
	Node *neighbour = cur->value;

	if(Hashmap_get(closedset, neighbour) == NULL) {
	  tentative_gscore =  current->gscore + 1;

	  if(!PQueue_contains(openset, neighbour) ||
	     tentative_gscore > neighbour->gscore) {
	    if(!PQueue_contains(openset, neighbour)) {
	      neighbour->came_from = current;
	      neighbour->gscore = tentative_gscore;
	      neighbour->fscore = neighbour->gscore + heuristic_cost_estimate(neighbour->point, destination);
	      PQueue_push(openset, neighbour, neighbour->fscore);
	    }
	  }
	}
      }

      List_destroy(neighbours);
    }
  }

  Hashmap_destroy(closedset);
  PQueue_destroy(openset);

  Hashmap_traverse(nodes, traverse_clean);
  free(destination);
  Hashmap_destroy(nodes);

  return result;
}
