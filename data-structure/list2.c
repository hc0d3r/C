#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct list {
    void *ptr;
    size_t n;
    size_t size;
} list_t;

void init_list(list_t *list, size_t size){
    list->size = size;
    list->ptr = NULL;
    list->n = 0;
}

int insert(list_t *list, void *element){
    void *aux;
    int ret;

    aux = realloc(list->ptr, (list->n+1) * list->size);
    if(aux == NULL){
        ret = 1;
        goto end;
    }

    memcpy((char *)aux+(list->n*list->size), element, list->size);

    list->ptr = aux;
    list->n++;
    ret = 0;

    end:
    return ret;
}

int main(void){
    list_t list;
    size_t i;

    init_list(&list, sizeof(size_t));

    for(i=0; i<10; i++){
        insert(&list, &i);
    }

    for(i=0; i<list.n; i++){
        printf("--> %zu\n", *(size_t *)list.ptr+i);
    }

    free(list.ptr);

    return 0;
}
