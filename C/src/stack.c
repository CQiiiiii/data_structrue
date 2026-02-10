#include "stack.h"

Stack *creat_Stack(value_dup_stack *dup_, value_free_stack *free_){
    if((dup_ == NULL) != (free_ == NULL)) return NULL;

    Stack *stack = (Stack *)malloc(sizeof(*stack));
    if(!stack) return NULL;
    memset(stack, 0, sizeof(*stack));
    stack->top = NULL;
    stack->size = 0;
    if(dup_) stack->dup_ = dup_;
    if(free_) stack->free_ = free_;

    return stack;
}

static void *stack_duplicate_or_assign(Stack *stack, void *data){
    if(!stack->dup_) return data;
    void *dup = stack->dup_(data);
    if(!dup){
        printf("Fail to dup data\n");
        return NULL;
    }
    return dup;
}

bool push(Stack *stack, void *data){
    if (!stack || !data) return false;

    StackNode *node = malloc(sizeof(*node));
    if (!node) return false;

    node->data = stack_duplicate_or_assign(stack, data);
    if(!node->data){
        free(node);
        return false;
    } 
    node->next = stack->top;
    stack->top = node;
    stack->size++;

    return true;
}

void *pop(Stack *stack){
    if(!stack || !stack->top) return NULL;
    StackNode *temp = stack->top;
    void *data = stack_duplicate_or_assign(stack, stack->top->data);
    if(!data){
        return NULL;
    }
    if(stack->free_) stack->free_(stack->top->data);
    stack->top = temp->next;
    stack->size--;

    free(temp);
    return data;
}

void *peek(Stack *stack){
    if(!stack || !stack->top) return NULL;;
    return stack->top->data;
}

bool is_empty(Stack *stack) {
    return stack == NULL || stack->top == NULL;
}

size_t get_size(Stack *stack) {
    return stack ? stack->size : 0;
}

void destroy_stack(Stack *stack){
    if(!stack) return;
    while(stack->top){
        void *data = pop(stack);
        if(stack->free_ && data) stack->free_(data);
    }
    free(stack);
}