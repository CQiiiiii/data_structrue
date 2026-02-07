#include "hashmap.h"

hashnode *creat_hashnode(hashmap *map, const char *key, void *value){
    hashnode *node = (hashnode *)malloc(sizeof(hashnode));
    if (!node) return NULL;

    node->key = strdup(key);
    if (!node->key){
        free(node);
        return NULL;
    }
    node->value = hash_duplicate_or_assign(map, value);
    if (map->dup_value && !node->value) {
        free(node->key);
        free(node);
        return NULL;
    }
    node->next = NULL;
    return node;
}

static void* hash_duplicate_or_assign(hashmap *map, void *value) {
    if (!map->dup_value) return value;
    
    void *dup = map->dup_value(value);
    if(!dup) {
        printf("fail to dup value");
        return value;
    }
    return dup;
}

uint hash_func(const char *key, int capacity){
    uint hash = 5381;
    int c;

    while ((c = *key++))
    {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % capacity;  
}

hashmap *hashmap_create(int initial_capacity, value_dup_fn dup_, value_free_fn free_){
    if (initial_capacity <= 0) return NULL;
    if ((dup_ == NULL) != (free_ == NULL)) return NULL;

    hashmap *map = (hashmap*)malloc(sizeof(hashmap));
    if(!map) return NULL;

    map->capacity = initial_capacity;
    map->size = 0;
    map->load_factor = 0.75;
    map->dup_value = dup_;
    map->free_value = free_;

    map->buckets = (hashnode **)calloc(initial_capacity, sizeof(hashnode*));
    if (!map->buckets){
        free(map);
        return NULL;
    }
    return map;
}

bool hashmap_put(hashmap *map, const char *key, void *value){
    if(!map || !key) return false;

    uint index = hash_func(key, map->capacity);
    
    hashnode *current = map->buckets[index];
    while(current){
        if(strcmp(current->key, key) == 0){
            if (map->free_value) map->free_value(current->value);
            current->value = hash_duplicate_or_assign(map, value);
            return true;
        }
        current = current->next;
    }

    hashnode *new_node = creat_hashnode(map, key, value);
    if (!new_node) return false;

    new_node->next = map->buckets[index];
    map->buckets[index] = new_node;
    map->size++;

    if ((float)map->size / map->capacity > map->load_factor){
        hashmap_resize(map, map->capacity * 2);
    }
    return true;
}

bool hashmap_get(hashmap *map, const char *key, void **value){
    if (!map || !key) return false;

    uint index = hash_func(key, map->capacity);
    hashnode *current = map->buckets[index];

    while(current){
        if(strcmp(current->key, key) == 0){
            if(value) *value = current->value;
            return true;
        }
        current = current->next;
    }
    return false;
}

bool hashmap_remove(hashmap *map, const char *key){
    if(!map || !key) return false;
    
    uint index = hash_func(key, map->capacity);
    hashnode *current = map->buckets[index];
    hashnode *prev = NULL;

    while(current){
        if(strcmp(current->key, key) == 0){
            if (prev){
                prev->next = current->next;
            }else{
                map->buckets[index] = current->next;
            }

            free(current->key);
            if (map->free_value) map->free_value(current->value);
            free(current);
            map->size--;
            return true;
        }
        prev = current;
        current = current->next;
    }

    return false;
}

void hashmap_resize(hashmap *map, int new_capacity){
    if(!map || new_capacity <= map->capacity) return;

    hashnode **new_buckets = (hashnode **)calloc(new_capacity, sizeof(hashnode*));
    if(!new_buckets) return;

    for (int i = 0; i < map->capacity; i++){
        hashnode *current = map->buckets[i];

        while (current){
            hashnode *next = current->next;

            uint new_index = hash_func(current->key, new_capacity);

            current->next = new_buckets[new_index];
            new_buckets[new_index] = current;

            current = next;
        }
    }

    free(map->buckets);
    map->buckets = new_buckets;
    map->capacity = new_capacity;
}

bool hashmap_contains(hashmap *map, const char *key){
    return hashmap_get(map, key, NULL);
}

void hashmap_clear(hashmap *map){
    if (!map) return;

    for(int i = 0; i < map->capacity; i++){
        hashnode *current = map->buckets[i];

        while(current){
            hashnode *next = current->next;
            free(current->key);
            if(map->free_value){
                map->free_value(current->value);
            }
            free(current);
            current = next;
        }
        map->buckets[i] = NULL;
    }
    map->size = 0;
}

void hashmap_destory(hashmap *map){
    if (!map) return;

    hashmap_clear(map);
    free(map->buckets);
    free(map);
}

void hashmap_print(hashmap *map){
    if(!map) return;

    printf("hashMap (size=%d, capacity=%d):\n", map->size, map->capacity);

    for(int i = 0; i < map->capacity; i++){
        if(map->buckets[i]){
            printf("Bucket[%d]: ", i);
            hashnode *current = map->buckets[i];

            while (current){
                printf("(%s:%p) ->", current->key, *(int *)current->value);
                current = current->next;
            }
            printf("NULL\n");
        }
    }
}