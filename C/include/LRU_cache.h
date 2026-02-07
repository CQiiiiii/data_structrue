#include <stdlib.h>
#include <stdbool.h>
#include "hashmap.h"

typedef void *(*value_dup_LRU)(const void *);
typedef void (*value_free_LRU)(void *);

typedef struct LRU_Node{
    void *data;
    char *index;
    struct LRU_Node *prev;
    struct LRU_Node *next;
}LRU_Node;

typedef struct LRU_Queue{
    LRU_Node *head;
    LRU_Node *tail;
    int size;
    int capacity;
    hashmap *keymap;
    value_dup_LRU LRU_dup;
    value_free_LRU LRU_free;
}LRU_Queue;


LRU_Node *creat_LRUnode(LRU_Queue *queue, char *key, void *data);
LRU_Queue *LRU_init(int capacity, value_dup_LRU dup_, value_free_LRU free_);
static void* LRU_duplicate_or_assign(LRU_Queue *queue, void *value);
bool LRU_add(LRU_Queue *queue, LRU_Node *node);
bool LRU_remove(LRU_Queue *queue, LRU_Node *node);
void LRU_free_node(LRU_Queue *queue, LRU_Node *node);
void LRU_delete_node(LRU_Queue *queue, LRU_Node *node);
void *LRU_get(LRU_Queue *queue, char *key);
bool LRU_put(LRU_Queue *queue, char *key, void *data);
void LRU_destroy(LRU_Queue *queue);
void LRU_clear(LRU_Queue *queue);