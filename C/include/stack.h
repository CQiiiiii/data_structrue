#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef void *(*value_dup_stack)(const void *);
typedef void (*value_free_stack)(void *);

typedef struct StackNode{
    void *data;
    struct StackNode *next;
}StackNode;

typedef struct Stack{
    StackNode *top;
    size_t size;
    value_dup_stack dup_;
    value_free_stack free_;
}Stack;

Stack *creat_Stack(value_dup_stack *dup_, value_free_stack *free_);
static void *stack_duplicate_or_assign(Stack *stack, void *data);
bool push(Stack *stack, void *data);
void *pop(Stack *stack);
void *peek(Stack *stack);
bool is_empty(Stack *stack);
size_t get_size(Stack *stack);
void destroy_stack(Stack *stack);