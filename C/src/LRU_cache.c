#include "LRU_cache.h"

LRU_Node *creat_LRUnode(LRU_Queue *queue, char *key, void *data){
    LRU_Node *node = (LRU_Node *)malloc(sizeof(LRU_Node));
    if (!node) return NULL;

    node->index = strdup(key);
    if(!node->index){
        free(node);
        return NULL;
    }
    node->data = LRU_duplicate_or_assign(queue, data);
    if (queue->LRU_dup && !node->data) {
        free(node->index);
        free(node);
        return NULL;
    }
    node->next = NULL;
    node->prev = NULL;

    return node;
}

static void* LRU_duplicate_or_assign(LRU_Queue *queue, void *value){
    if(!queue->LRU_dup) return value;

    void *dup = queue->LRU_dup(value);
    if(!dup){
        printf("LRU:Fail to dup value.\n");
        return value;
    }
    return dup;
}

LRU_Queue *LRU_init(int capacity, value_dup_LRU dup_, value_free_LRU free_){
    if (capacity <= 0) return NULL;
    if ((dup_ == NULL) != (free_ == NULL)) return NULL;

    LRU_Queue *queue = (LRU_Queue *)malloc(sizeof(LRU_Queue));
    if(!queue) return NULL;

    LRU_Node *head_dummy = (LRU_Node *)malloc(sizeof(LRU_Node));
    if(!head_dummy) {
        free(queue);
        return NULL;
    }

    LRU_Node *tail_dummy = (LRU_Node *)malloc(sizeof(LRU_Node));
    if(!tail_dummy) {
        free(head_dummy);
        free(queue);
        return NULL;
    }
    
    hashmap *keymap = hashmap_create(8, NULL, NULL);
    if(!keymap){
        free(tail_dummy);
        free(head_dummy);
        free(queue);
        return NULL;
    }

    head_dummy->next = tail_dummy;
    head_dummy->prev = NULL;
    head_dummy->data = NULL;
    head_dummy->index = NULL;

    tail_dummy->next = NULL;
    tail_dummy->prev = head_dummy;
    tail_dummy->data = NULL;
    tail_dummy->index = NULL;

    queue->head = head_dummy;
    queue->tail = tail_dummy;
    queue->capacity = capacity;
    queue->size = 0;
    queue->keymap = keymap;
    queue->LRU_dup = dup_;
    queue->LRU_free = free_;

    return queue;
}

bool LRU_add(LRU_Queue *queue, LRU_Node *node){
    //add the node to head
    if (!queue || !node) return false;

    LRU_Node *current = queue->head->next;
    node->prev = queue->head;
    node->next = current;

    queue->head->next = node;

    current->prev = node;

    queue->size++;
    return true;
}

bool LRU_remove(LRU_Queue *queue, LRU_Node *node){
    if (!queue || !node) return false;
    if (node == queue->head || node == queue->tail) {
        printf("illegal node.\n");
        return false;
    }
    LRU_Node *prev = node->prev;
    LRU_Node *next = node->next;

    prev->next = next;
    next->prev = prev;

    node->prev = NULL;
    node->next = NULL;
    queue->size--;
    return true;
}

void LRU_free_node(LRU_Queue *queue, LRU_Node *node){
    if (!node) return;
    
    free(node->index);
    if (queue->LRU_free) {
        queue->LRU_free(node->data);
    }
    free(node);
}

void LRU_delete_node(LRU_Queue *queue, LRU_Node *node){
    if (!queue || !node) return;
    LRU_remove(queue, node);
    LRU_free_node(queue, node);
}

void *LRU_get(LRU_Queue *queue, char *key){
    if (!queue || !key) return NULL;
    if(hashmap_contains(queue->keymap, key)){
        LRU_Node *node;
        hashmap_get(queue->keymap, key, (void **)&node);
        LRU_remove(queue, node);
        LRU_add(queue, node);

        return node->data;
    }
    return NULL;
}

bool LRU_put(LRU_Queue *queue, char *key, void *data){
    if(!queue || !key) return false;
    if(hashmap_contains(queue->keymap, key)){
        LRU_Node *existing;
        hashmap_get(queue->keymap, key, (void **)&existing);
        if(queue->LRU_free){
            queue->LRU_free(existing->data);
        }
        existing->data = LRU_duplicate_or_assign(queue, data);
        if (queue->LRU_dup && !existing->data) return false;

        LRU_remove(queue, existing);
        LRU_add(queue, existing);
        return true;
    }

    if(queue->size >= queue->capacity){
        LRU_Node *last = queue->tail->prev;
        if (last != queue->head){
            hashmap_remove(queue->keymap, last->index);
            LRU_delete_node(queue, last);
        }
    }
    LRU_Node *node = creat_LRUnode(queue, key, data);
    if(!node) return false;

    LRU_add(queue, node);
    hashmap_put(queue->keymap, key, node);
    
    return true;
}

void LRU_destroy(LRU_Queue *queue){
    if (!queue) return;

    LRU_Node *current = queue->head;
    while(current){
        LRU_Node *next = current->next;
        LRU_free_node(queue, current);
        current = next;
    }

    hashmap_destory(queue->keymap);
    free(queue);
}

void LRU_clear(LRU_Queue *queue){
    if(!queue) return;

    LRU_Node *current = queue->head->next;
    while(current != queue->tail){
        LRU_Node *next = current->next;
        hashmap_remove(queue->keymap, current->index);
        LRU_free_node(queue, current);
        current = next;
    }

    queue->head->next = queue->tail;
    queue->tail->prev = queue->head;
    queue->size = 0;
}