//-----------------------------------------------------------------------------
// Imports
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//-----------------------------------------------------------------------------
// Node
//-----------------------------------------------------------------------------

struct sNode
{
	struct sNode * prev;
	struct sNode * next;
    char value;
};
typedef struct sNode Node;

Node * node_alloc_n(int nb)
{
    Node * nodes = malloc(sizeof(Node) * nb);
    Node * prev = NULL;
    for (int n=0; n < nb; n++)
    {
        nodes[n].next = &nodes[n+1];
        nodes[n].prev = prev;
        nodes[n].value = '\0';
        prev = &nodes[n];
    }
    nodes[nb-1].next = NULL;
    return nodes;
}

//-----------------------------------------------------------------------------
// Liste
//-----------------------------------------------------------------------------

typedef struct 
{
	int size;
    int allocated_size;
	Node * tail;
	Node * head;
} List ;

List * list_create(void)
{
    List * ls = malloc(sizeof(List));
    Node * nodes = node_alloc_n(4);
    ls->head = nodes;
    ls->tail = nodes; //&(nodes[3]);
    ls->size = 0;
    ls->allocated_size = 4;
    return ls;
}

bool list_is_empty(List * ls)
{
    if (ls->size == 0 && ls->head == ls->tail)
    {
        return true;
    }
    return false;
}

void list_append(List * ls, char v)
{
    if (list_is_empty(ls))
    {
        ls->tail->value = v;
        ls->size += 1;
    }
    else
    {
        if (ls->size >= ls->allocated_size)
        {
            ls->tail->next = node_alloc_n(ls->allocated_size * 2);
            ls->tail->next->prev = ls->tail;
        }
        ls->tail = ls->tail->next;
        ls->tail->value = v;
        ls->size += 1;
    }
}

char list_get(List * ls, int i)
{
    if (i < (ls->size >> 1))
    {
        // on parcourt depuis le début
        Node * start = ls->head;
        int cpt = 1;
        while (start != NULL)
        {
            if (i == cpt)
            {
                return start->value;
            }
            start = start->next;
            cpt += 1;
        }
        // ERROR
    }
    else
    {
        // on parcourt depuis la fin
        Node * start = ls->tail;
        int cpt = ls->size;
        while (start != NULL)
        {
            if (i == cpt)
            {
                return start->value;
            }
            start = start->prev;
            cpt -= 1;
        }
        // ERROR
    }
    return '\0';
}

int list_size(List * ls)
{
    if (ls != NULL)
    {
        return ls->size;
    }
    return 0; // Handle error
}

void list_free(List * ls)
{
    //Node * tail = ls->tail;
}

char list_pop(List * ls)
{
    char value = ls->tail->value;
    ls->tail->value = '\0';
    if (ls->size > 1)
    {
        ls->tail = ls->tail->prev;
    }
    ls->size -= 1;
    return value;
}

void list_reverse(List * ls)
{
    Node * start = ls->head;
    ls->head = ls->tail;
    ls->tail = start;
    while (start != NULL)
    {
        Node * next = start->next;
        start->next = start->prev;
        start->prev = next;
        start = next;
    }
}

//-----------------------------------------------------------------------------
// Main function
//-----------------------------------------------------------------------------

int main(void)
{
	List * myList = list_create();
    printf("The size of my list is %d\n", list_size(myList));
    if (list_is_empty(myList))
    {
        printf("This list is empty.\n");
    }
    list_append(myList, 'A');
    printf("The size of my list is %d\n", list_size(myList));
    list_append(myList, 'B');
    list_append(myList, 'C');
    list_append(myList, 'D');
    list_append(myList, 'E');
    list_reverse(myList);
    printf("The size of my list is %d\n", list_size(myList));
    for (int i = 1; i <= list_size(myList); i += 1)
    {
        printf("%d. %c\n", i, list_get(myList, i));
    }
    char popped = list_pop(myList);
    printf("Char popped = %c\n", popped);
    for (int i = 1; i <= list_size(myList); i += 1)
    {
        printf("%d. %c\n", i, list_get(myList, i));
    }
	printf("Hello World\n");
	//free(myList);
	return 0;
}
