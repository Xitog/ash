#ifndef __LIST__
#define __LIST__

//-----------------------------------------------------------------------------
// Imports
//-----------------------------------------------------------------------------

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "value.h"

//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------

typedef struct _ListElement {
    AshRef node;
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
unsigned int list_append(List * list, AshRef ref);
AshRef list_get(List * list, unsigned int index);
AshRef list_pop(List * list);
void list_free(List * list);
unsigned int  list_size(List * list);
bool list_is_empty(List * list);
void list_reverse(List * list);
void list_print(List * list);
void list_free(List * list);

#endif
