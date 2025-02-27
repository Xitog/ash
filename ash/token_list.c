#include "token_list.h"

TokenList *token_list_init()
{
    TokenList *list = (TokenList *)memory_get(sizeof(TokenList));
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
    return list;
}

uint32_t token_list_append(TokenList *list, Token tok)
{
    TokenListElement *elem = (TokenListElement *)memory_get(sizeof(TokenListElement));
    elem->token = tok;
    elem->next = NULL;
    elem->prev = NULL;
    // Empty list
    if (list->head == NULL)
    {
        list->head = elem;
        list->tail = elem;
    }
    else
    {
        list->tail->next = elem;
        elem->prev = list->tail;
        list->tail = elem;
    }
    list->count += 1;
    return list->count;
}

Token token_list_get(TokenList *list, uint32_t index)
{
    if (index >= list->count)
    {
        printf("Index %d out of boundaries [0, %d]", index, list->count);
        general_error("Index out of boundaries");
    }
    // On divise par deux la taille pour savoir si commence par le début ou la fin
    TokenListElement *current = list->head;
    unsigned int count = 0;
    int direction = 1;
    if (index >= (list->count >> 1))
    {
        count = list->count - 1;
        direction = -1;
        current = list->tail;
    }
    //printf("token_list_get(%d) : start at %d direction %d\n", index, count, direction);
    while (current != NULL && count != index)
    {
        if (direction == 1)
        {
            current = current->next;
        }
        else
        {
            current = current->prev;
        }
        count += direction;
    }
    return current->token;
}

Token token_list_pop(TokenList *list)
{
    if (list->count <= 0)
    {
        general_error("Empty list");
    }
    Token tok;
    TokenListElement *last = list->tail;
    tok = last->token;
    if (list->tail == list->head) {
        list->tail = NULL;
        list->head = NULL;
    } else {
        list->tail = last->prev;
        list->tail->next = NULL;
    }
    list->count -= 1;
    memory_free(last);
    return tok;
}

uint32_t token_list_size(TokenList *list)
{
    if (list == NULL)
    {
        return 0;
    }
    return list->count;
}

bool token_list_is_empty(TokenList *list)
{
    if (list->count == 0)
    {
        return true;
    }
    return false;
}

void token_list_reverse(TokenList *list)
{
    TokenListElement *start = list->head;
    list->head = list->tail;
    list->tail = start;
    while (start != NULL)
    {
        TokenListElement *next = start->next;
        start->next = start->prev;
        start->prev = next;
        start = next;
    }
}

void token_list_print(TokenList *list)
{
    TokenListElement *start = list->head;
    unsigned int count = 0;
    while (start != NULL)
    {
        token_print(start->token);
        start = start->next;
        count++;
    }
}

void token_list_free(TokenList *list)
{
    TokenListElement *current = list->head;
    while (current != NULL)
    {
        TokenListElement *old = current;
        current = current->next;
        memory_free(old);
    }
    memory_free(list);
}
