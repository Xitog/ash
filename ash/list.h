#ifndef __LIST__
#define __LIST__

//-----------------------------------------------------------------------------
// Imports
//-----------------------------------------------------------------------------

#include "value.h"

//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------

typedef struct _ListElement {
    Value node;
    struct _ListElement * next;
    struct _ListElement * prev;
} ListElement;

typedef struct _List {
    ListElement * head;
    ListElement * tail;
    unsigned int count;
    ValueType elem_type;
} List;

typedef struct _Iterator {
    List * list;
    int index;
} Iterator;

//-----------------------------------------------------------------------------
// Fonctions
//-----------------------------------------------------------------------------

List * list_init(ValueType t);
unsigned int list_append(List * list, Value val);
Value list_get(List * list, int index);
void list_set(List * list, int index, Value val);
// list_insert :TODO:
Value list_pop(List * list);
void list_free(List * list);
size_t list_size(List * list);
bool list_is_empty(List * list);
void list_reverse(List * list);
//void list_print(List * list, void (*print)());
void list_print(List * list);
void list_free(List * list);

Iterator iterator_init(List * list);
Value iterator_next(Iterator * it);

#endif
