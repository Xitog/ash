#include "list.h"

List * list_init() {
    List * list = (List *) malloc(sizeof(List));
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
    return list;
}

unsigned int list_append(List * list, void * e) {
    ListElement * elem = (ListElement *) malloc(sizeof(ListElement));
    elem->node = e;
    elem->next = NULL;
    if (list->head == NULL) {
        list->head = elem;
        list->tail = elem;
    } else {
        list->tail->next = elem;
        list->tail = elem;
    }
    list->count += 1;
    return list->count;
}

void * list_get(List * list, unsigned int index) {
    unsigned int count = 0;
    ListElement * current = list->head;
    if (index >= list->count) {
        return NULL;
    }
    while (count < index) {
        current = current->next;
        count += 1;
    }
    return current;
}

void list_free(List * list) {
    ListElement * current = list->head;
    while (current != NULL) {
        if (current->node != NULL) {
            free(current->node);
        }
        ListElement * old = current;
        current = current->next;
        free(old);
    }
    free(list);
}


