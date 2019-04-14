#include <stdio.h>
#include <stdlib.h>

typedef struct stack {
    void *data;
    struct stack *next;
} stack_t;

int push(stack_t **stack, void *v){
    stack_t *tmp;
    int ret;

    tmp = malloc(sizeof(stack_t));
    if(tmp == NULL){
        ret = 1;
        goto end;
    }

    tmp->data = v;
    tmp->next = *stack;
    *stack = tmp;

    ret = 0;

    end:
    return ret;
}

stack_t *pop(stack_t **stack){
    stack_t *ret;

    if(*stack == NULL){
        ret = NULL;
        goto end;
    }

    ret = *stack;
    *stack = ret->next;


    end:
    return ret;
}

int main(void){
    stack_t *stack = NULL, *element;
    size_t i;

    for(i=0; i<5; i++){
        push(&stack, (void *)i);
        push(&stack, (void *)(i*2));
    }

    while((element = pop(&stack))){
        printf("value => %zu\n", (size_t)element->data);
        free(element);
    }

    return 0;
}
