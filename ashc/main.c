//-----------------------------------------------------------------------------
// Imports
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------

typedef enum
{
    TYPE_INTEGER = 1,
    TYPE_FLOAT   = 2,
    TYPE_BOOLEAN = 3,
    TYPE_STRING  = 4,
    TYPE_ARRAY   = 5,
    TYPE_LIST    = 6,
    TYPE_TABLE   = 7,
    TYPE_RECORD  = 8
} AshType;

typedef union
{
    long   i;   // 32
    float  d;   // 32
    bool   b;
    void * p;
} AshValue;

typedef struct
{
    AshType type;
    AshValue value;
} AshObject;

typedef struct
{
    long size;
    char * content;
} AshString;

void print(AshObject * o)
{
    if (o != NULL)
    {
        if (o->type == TYPE_STRING)
        {
            printf("%s (%d)\n", ((AshString *)o->value.p)->content, ((AshString *)o->value.p)->size);
        }
        else if (o->type == TYPE_INTEGER)
        {
            printf("%d\n", o->value.i);
        }
        else if (o->type == TYPE_BOOLEAN)
        {
            if (o->value.b == true)
            {
                printf("true\n");
            }
            else
            {
                printf("false\n");
            }
        }
        else
        {
            printf("ERREUR : TYPE INCONNU\n");
        }
    }
}

AshObject * create_string(char * source, long source_size)
{
    AshString * s = malloc(sizeof(AshString));
    s->size = source_size;
    s->content = malloc(sizeof(char) * source_size);
    for (int i = 0; i < source_size; i++)
    {
        s->content[i] = source[i];
    }

    AshObject * o = malloc(sizeof(AshObject));
    o->value.p = (void *) s;
    o->type = TYPE_STRING;

    return o;
}

AshObject * create_integer(long i)
{
    AshObject * o = malloc(sizeof(AshObject));
    o->value.i = i;
    o->type = TYPE_INTEGER;
}

AshObject * create_boolean(bool b)
{
    AshObject * o = malloc(sizeof(AshObject));
    o->value.b = b;
    o->type = TYPE_BOOLEAN;
}

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

    printf("Test object:\n");
    AshObject * o1 = create_string("Bonjour le monde", 17);
    print(o1);
    AshObject * o2 = create_integer(23);
    print(o2);
    AshObject * o3 = create_boolean(true);
    print(o3);

	return 0;
}
