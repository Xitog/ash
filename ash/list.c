#include "list.h"

List * list_init(Type elem_type) {
    List * list = (List *) memory_get(sizeof(List));
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
    list->elem_type = elem_type;
    return list;
}

unsigned int list_append(List * list, Value val) {
    if (val.type != list->elem_type)
    {
        general_error("Impossible to add type %s to list of %s", TYPE_REPR_STRING[val.type], TYPE_REPR_STRING[list->elem_type]);
    }
    ListElement * elem = (ListElement *) memory_get(sizeof(ListElement));
    if (is_primitive(val))
    {
        elem->node = val;
    }
    else
    {
        // :TODO:
        // elem->node = memory_get(memory_size(data));
        // memory_copy(elem->node, data);
    }
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

Value list_get(List * list, int index) {
    // :TODO: negative index
    //printf("list_get at %d on list of size %d\n", index, list_size(list));
    unsigned int u_index = (unsigned int) index;
    if (u_index > 0 && u_index >= list->count) {
        return NIL;
    }
    // On divise par deux la taille pour savoir si commence par le début ou la fin
    ListElement * current = list->head;
    unsigned int count = 0;
    int direction = 1;
    if (u_index >= (list->count >> 1))
    {
        count = list->count - 1;
        direction = -1;
        current = list->tail;
    }
    while (current != NULL && count != u_index) {
        //printf("searching at %d direction %d\n", count, direction);
        if (direction == 1) {
            current = current->next;
        } else {
            current = current->prev;
        }
        count += direction;
    }
    return current->node;
}

void list_set(List * list, int index, Value val) {
    // :TODO: negative index
    unsigned int u_index = (unsigned int) index;
    if (u_index > 0 && u_index >= list->count) {
        general_error("Cannot set at %d index for a list of size %d", index, list->count);
    }
    // On divise par deux la taille pour savoir si commence par le début ou la fin
    ListElement * current = list->head;
    unsigned int count = 0;
    int direction = 1;
    if (u_index >= (list->count >> 1))
    {
        count = list->count - 1;
        direction = -1;
        current = list->tail;
    }
    //printf("start at %d direction %d\n", count, direction);
    while (current != NULL && count != u_index) {
        if (direction == 1) {
            current = current->next;
        } else {
            current = current->prev;
        }
        count += direction;
    }
    if (is_primitive(current->node))
    {
        current->node = val;
    }
    else
    {
        // :TODO:
    }
}

Value list_pop(List * list)
{
    Value value = NIL;
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

size_t list_size(List * list) {
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

void list_print(List * list)
{
    size_t size = list_size(list);
    printf("List <%s> [#%d] :\n", TYPE_REPR_STRING[list->elem_type], size);
    Iterator it = iterator_init(list);
    Value current = iterator_next(&it);
    unsigned int count = 0;
    while (!is_nil(current) && count < size)
    {
        value_print_message("\t%d. %v\n", count, current);
        count += 1;
        current = iterator_next(&it);
    }
}

/*
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
*/

void list_free(List * list) {
    ListElement * current = list->head;
    while (current != NULL) {
        if (is_primitive(current->node))
        {
            // do nothing;
        }
        else
        {
            // : TODO:
            // memory_free(current->node);
        }
        ListElement * old = current;
        current = current->next;
        memory_free(old);
    }
    memory_free(list);
}

Iterator iterator_init(List * list)
{
    Iterator it;
    it.index = -1;
    it.list = list;
    return it;
}

Value iterator_next(Iterator * it)
{
    if (it->list->count == 0)
    {
        return NIL;
    }
    it->index += 1;
    Value v = list_get(it->list, it->index);
    return v;
}
