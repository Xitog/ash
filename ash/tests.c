#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

int main(int argc, char *argv[])
{
    List * list = list_init();
    int a = 5;
    int b = 10;
    int c = 32;
    list_append(list, (void *) &a);
    list_append(list, (void *) &b);
    list_append(list, (void *) &c);
    printf("This a list (5, 10, 32):\n");
    list_print(list);
    printf("Its size is : %d / 3\n", list_size(list));
    printf("Access to the third element: %d = 32\n", *((int *) list_get(list, 2)));
    return EXIT_SUCCESS;
}
