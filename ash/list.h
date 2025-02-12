#ifndef __LIST__
#define __LIST__

//-----------------------------------------------------------------------------
// Imports
//-----------------------------------------------------------------------------

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "general.h"
#include "value.h"

//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------

typedef struct _ListElement {
    void * node;
    struct _ListElement * next;
    struct _ListElement * prev;
} ListElement;

typedef struct _List {
    ListElement * head;
    ListElement * tail;
    unsigned int count;
} List;

//-----------------------------------------------------------------------------
// Fonctions
//-----------------------------------------------------------------------------

List * list_init();
unsigned int list_append(List * list, void * data);
void * list_get(List * list, unsigned int index);
void * list_pop(List * list);
void list_free(List * list);
unsigned int  list_size(List * list);
bool list_is_empty(List * list);
void list_reverse(List * list);
void list_print(List * list, void (*print)());
void list_free(List * list);

#endif
