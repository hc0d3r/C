#include <stdio.h>
#include <stdlib.h>

struct node {
    void *data;
    struct node *next;
};

struct queue {
    struct node *head;
    struct node **end;
};

void init_queue(struct queue *q){
    q->head = NULL;
    q->end = &(q->head);
}

void insert(struct queue *q, void *data){
    struct node *aux = malloc(sizeof(struct node));
    aux->data = data;
    aux->next = NULL;

    *(q->end) = aux;
    q->end = &(aux->next);
}

struct node *dequeue(struct queue *q){
    struct node *head = q->head;

    if(!head)
        return NULL;

    q->head = q->head->next;

    if(!q->head)
        q->end = &(q->head);

    return head;
}


int main(void){
    struct node *aux;
    struct queue q;
    size_t i;

    init_queue(&q);

    for(i=0; i<10; i++){
        insert(&q, (void *)i);
    }

    while((aux = dequeue(&q))){
        printf("--> %zu\n", (size_t)aux->data);
        free(aux);
    }

    return 0;
}
