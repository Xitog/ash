#include "list.h"

List * list_init() {
    List * list = (List *) memory_get(sizeof(List));
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
    return list;
}

unsigned int list_append(List * list, void * data) {
    ListElement * elem = (ListElement *) memory_get(sizeof(ListElement));
    elem->node = memory_get(memory_size(data));
    memory_copy(elem->node, data);
    elem->next = NULL;
    elem->prev = NULL;
    // Empty list
    if (list->head == NULL) {
        list->head = elem;
        list->tail = elem;
    } else {
        list->tail->next = elem;
        elem->prev = list->tail;
        list->tail = elem;
    }
    list->count += 1;
    return list->count;
}

void * list_get(List * list, unsigned int index) {
    if (index >= list->count) {
        return NULL;
    }
    // On divise par deux la taille pour savoir si commence par le début ou la fin
    ListElement * current = list->head;
    unsigned int count = 0;
    int direction = 1;
    if (index >= (list->count >> 1))
    {
        count = list->count - 1;
        direction = -1;
        current = list->tail;
    }
    printf("start at %d direction %d\n", count, direction);
    while (current != NULL && count != index) {
        if (direction == 1) {
            current = current->next;
        } else {
            current = current->prev;
        }
        count += direction;
    }
    return current->node;
}

void *  list_pop(List * list)
{
    void * value = NULL;
    if (list->count > 0)
    {
        ListElement * last = list->tail;
        value = last->node;
        list->tail = last->prev;
        list->tail->next = NULL;
        list->count -= 1;
        memory_free(last);
    }
    return value;
}

unsigned int list_size(List * list) {
    if (list == NULL) {
        return 0;
    }
    return list->count;
}

bool list_is_empty(List * list) {
    if (list->count == 0) {
        return true;
    }
    return false;
}

void list_reverse(List * list)
{
    ListElement * start = list->head;
    list->head = list->tail;
    list->tail = start;
    while (start != NULL)
    {
        ListElement * next = start->next;
        start->next = start->prev;
        start->prev = next;
        start = next;
    }
}

void list_print(List * list, void (*print)())
{
    ListElement * start = list->head;
    unsigned int count = 0;
    while (start != NULL)
    {
        print(start->node);
        start = start->next;
        count++;
    }
}

void list_free(List * list) {
    ListElement * current = list->head;
    while (current != NULL) {
        memory_free(current->node);
        ListElement * old = current;
        current = current->next;
        memory_free(old);
    }
    memory_free(list);
}
