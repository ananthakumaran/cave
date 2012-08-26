#undef NDEBUG
#include <stdint.h>
#include "hashmap.h"
#include "dbg.h"

Hashmap *Hashmap_create(Hashmap_compare compare, Hashmap_hash hash)
{
  Hashmap *map = calloc(1, sizeof(Hashmap));
  check_mem(map);

  map->compare = compare;
  map->hash = hash;
  map->buckets = DArray_create(sizeof(DArray *), DEFAULT_NUMBER_OF_BUCKETS);
  check_mem(map);
  map->buckets->end = map->buckets->max; // fake out expanding it

  return map;

 error:
  if(map) Hashmap_destroy(map);
  return NULL;
}

void Hashmap_destroy(Hashmap *map) {
  int i = 0;
  int j = 0;
  if(map) {
    if(map->buckets) {
      for(i = 0; i < DArray_count(map->buckets); i++) {
	DArray *bucket = DArray_get(map->buckets, i);
	if(bucket) {
	  for(j = 0; j < DArray_count(bucket); j++) {
	    free(DArray_get(bucket, j));
	  }
	  DArray_destroy(bucket);
	}
      }
      DArray_destroy(map->buckets);
    }
    free(map);
  }
}

static inline HashmapNode *Hashmap_node_create(uint32_t hash, void *key, void *data)
{
  HashmapNode *node = calloc(1, sizeof(HashmapNode));
  check_mem(node);

  node->hash = hash;
  node->key = key;
  node->data = data;

  return node;
 error:
  return NULL;
}

static inline DArray *Hashmap_find_bucket(Hashmap *map, void *key, int create, uint32_t *hash_out)
{
  uint32_t hash = map->hash(key);
  int bucket_n = hash % DEFAULT_NUMBER_OF_BUCKETS;
  check(bucket_n >= 0, "Invalid bucket found: %d", bucket_n);
  *hash_out = hash;

  DArray *bucket = DArray_get(map->buckets, bucket_n);

  if(!bucket && create) {
    bucket = DArray_create(sizeof(void *), DEFAULT_NUMBER_OF_BUCKETS);
    check_mem(bucket);
    DArray_set(map->buckets, bucket_n, bucket);
  }

  return bucket;

 error:
  return NULL;
}

int Hashmap_set(Hashmap *map, void *key, void *data)
{
  uint32_t hash = 0;
  DArray *bucket = Hashmap_find_bucket(map, key, 1, &hash);
  check(bucket, "Error can't create bucket");

  HashmapNode *node = Hashmap_node_create(hash, key, data);
  DArray_push(bucket, node);

  return 0;
 error:
  return -1;
}

static inline int Hashmap_get_node(Hashmap *map, uint32_t hash, DArray *bucket, void *key)
{
  int i = 0;

  for(i = 0; i < DArray_count(bucket); i++) {
    HashmapNode *node = DArray_get(bucket, i);
    if(node->hash == hash && map->compare(node->key, key) == 0) {
      return i;
    }
  }

  return -1;
}


void *Hashmap_get(Hashmap *map, void *key) {

  uint32_t hash = 0;
  int n;

  DArray *bucket = Hashmap_find_bucket(map, key, 0, &hash);
  if(!bucket) return NULL;

  n = Hashmap_get_node(map, hash, bucket, key);
  if(n == -1) return NULL;

  HashmapNode *node = DArray_get(bucket, n);
  check(node, "Failed to get node from the bucket when it should exist");

  return node->data;

 error:
  return NULL;
}

int Hashmap_traverse(Hashmap *map, Hashmap_traverse_cb traverse_cb)
{
  int i = 0;
  int j = 0;
  int rc = 0;

  for(i = 0; i < DArray_count(map->buckets); i++) {
    DArray *bucket = DArray_get(map->buckets, i);
    if(bucket) {
      for(j = 0; j < DArray_count(bucket); j++) {
	HashmapNode *node = DArray_get(bucket, j);
	rc = traverse_cb(node);
	if(rc != 0) return rc;
      }
    }
  }

  return 0;
}

void *Hashmap_delete(Hashmap *map, void *key)
{
  uint32_t hash = 0;
  DArray *bucket = Hashmap_find_bucket(map, key, 0, &hash);
  if(!bucket) return NULL;

  int i = Hashmap_get_node(map, hash, bucket, key);
  if(i == -1) return NULL;

  HashmapNode *node = DArray_get(bucket, i);
  void *data = node->data;

  HashmapNode *end = DArray_pop(bucket);

  if(end != node) {
    // alright, looks like this is not the last one, swap it
    DArray_set(bucket, i, end);
  }

  free(node);
  return data;
}
