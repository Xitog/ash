//-----------------------------------------------------------------------------
// Imports
//-----------------------------------------------------------------------------

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "value.h"

//-----------------------------------------------------------------------------
// Main function
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    AshRef ref1 = integer_init(5);
    print(ref1);
    AshRef ref2 = float_init(22.3f);
    print(ref2);
    AshRef ref3 = boolean_init(false);
    print(ref3);
    AshRef ref4 = string_init("hello!", 6);
    print(ref4);

    List * list = list_init();
    printf("Its size is : %d / 0\n", list_size(list));
    if (list_is_empty(list))
    {
        printf("This list is empty.\n");
    }
    AshRef l1e1 = integer_init(5);
    AshRef l1e2 = integer_init(10);
    AshRef l1e3 = integer_init(32);
    list_append(list, l1e1);
    list_append(list, l1e2);
    list_append(list, l1e3);
    printf("This a list (5, 10, 32):\n");
    list_print(list);
    printf("Its size is : %d / 3\n", list_size(list));
    printf("Access to the third element:\n");
    print(list_get(list, 2));
    printf("Reversing...\n");
    list_reverse(list);
    list_print(list);
    printf("Its size is : %d / 3\n", list_size(list));
    printf("Access to the third element:\n");
    print(list_get(list, 2));
    List * list2 = list_init();
    AshRef l2e1 = string_init("a", 2);
    AshRef l2e2 = string_init("b", 2);
    AshRef l2e3 = string_init("c", 2);
    list_append(list2, l2e1);
    list_append(list2, l2e2);
    list_append(list2, l2e3);
    list_print(list2);
    AshRef popped = list_pop(list2);
    printf("Last char popped is:\n");
    print(popped);
    printf("List size is now: %d\n", list_size(list2));
    printf("List is now:\n");
    list_print(list2);
    return EXIT_SUCCESS;
}
