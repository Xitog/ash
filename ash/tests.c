//-----------------------------------------------------------------------------
// Imports
//-----------------------------------------------------------------------------

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "general.h"
#include "value.h"
#include "list.h"

#include "token.h"
#include "token_list.h"

//-----------------------------------------------------------------------------
// Main function
//-----------------------------------------------------------------------------

void test_list()
{
    printf("Start of test_list()");
    int *a = memory_get(sizeof(int));
    *a = 5;
    int *b = memory_get(sizeof(int));
    *b = 10;
    int *c = memory_get(sizeof(int));
    *c = 32;
    List *list = list_init();
    printf("Its size is : %d / 0\n", list_size(list));
    printf("Adding first element\n");
    list_append(list, a);
    printf("Adding second element\n");
    list_append(list, b);
    printf("Adding third element\n");
    list_append(list, c);
    printf("Its size is : %d / 3\n", list_size(list));
    memory_summary();
    printf("Freeing a\n");
    memory_free(a);
    printf("Freeing b\n");
    memory_free(b);
    printf("Freeing c\n");
    memory_free(c);
    list_free(list);
    memory_summary();
    printf("End of test_list()");
}

int main(int argc, char *argv[])
{
    printf("Running with %d parameters\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("%d. %s\n", i, argv[i]);
    }

    //test_list();

    printf("Test de Token:\n");
    char * text = "a + 5";
    Token t1 = {.start=0, .count=1, .text = text, .type=TOKEN_IDENTIFIER};
    Token t2 = {.start=2, .count=1, .text = text, .type=TOKEN_OPERATOR};
    Token t3 = {.start=4, .count=1, .text = text, .type=TOKEN_DECIMAL};
    token_print(t1);
    token_print(t2);
    token_print(t3);

    printf("Test d'une TokenList:\n");
    TokenList * tkl = token_list_init();
    token_list_append(tkl, t1);
    token_list_append(tkl, t2);
    token_list_append(tkl, t3);
    token_list_print(tkl);

    while (token_list_size(tkl) > 0) {
        printf("------------------------\n");
        printf("Size = %d\n", token_list_size(tkl));
        Token tx1 = token_list_pop(tkl);
        printf("Popped token:\n");
        token_print(tx1);
        printf("Rest:\n");
        token_list_print(tkl);
    }
    printf("Size = %d\n", token_list_size(tkl));

    token_list_free(tkl);
    memory_summary();

    return EXIT_SUCCESS;

    /*
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
    */
}
