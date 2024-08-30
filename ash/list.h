#ifndef __LIST__
#define __LIST__

#include <stdlib.h>

typedef struct _ListElement {
    void * node;
    struct _ListElement * next;
} ListElement;

typedef struct _List {
    ListElement * head;
    ListElement * tail;
    unsigned int count;
} List;

List * list_init();
unsigned int list_append(List * list, void * e);
void * list_get(List * list, unsigned int index);
void list_free(List * list);

#endif
