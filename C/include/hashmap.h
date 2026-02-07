#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//基于链地址法实现

typedef void *(*value_dup_fn)(const void *);
typedef void (*value_free_fn)(void *);

typedef struct hashnode{
    char *key;
    void *value;
    struct hashnode *next;
}hashnode;

typedef unsigned int uint;

typedef struct hashmap{
    hashnode **buckets;
    int capacity;
    int size;
    float load_factor;
    value_free_fn free_value;
    value_dup_fn dup_value;
}hashmap;

static void* hash_duplicate_or_assign(hashmap *map, void *value);
hashnode *creat_Hashnode(const char *key, void *value);
bool hashmap_set_free_dup(hashmap *map, value_dup_fn dup_, value_free_fn free_);
uint hash_func(const char *key, int capacity);
hashmap *hashmap_create(int initial_capacity, value_dup_fn dup_, value_free_fn free_);
bool hashmap_put(hashmap *map, const char *key, void *value);
bool hashmap_get(hashmap *map, const char *key, void **value);
bool hashmap_remove(hashmap *map, const char *key);
void hashmap_resize(hashmap *map, int new_capacity);
bool hashmap_contains(hashmap *map, const char *key);
void hashmap_clear(hashmap *map);
void hashmap_destory(hashmap *map);
void hashmap_print(hashmap *map);