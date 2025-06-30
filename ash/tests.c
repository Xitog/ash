//-----------------------------------------------------------------------------
// Imports
//-----------------------------------------------------------------------------

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "general.h"
#include "value.h"
#include "list.h"
#include "dict.h"

#include "token.h"
#include "token_list.h"

//-----------------------------------------------------------------------------
// Main function
//-----------------------------------------------------------------------------

void test_list()
{
    printf("Start of test_list()\n");
    memory_summary();
    printf("Creating objets\n");
    Value a = integer_init(5);
    Value b = integer_init(10);
    Value c = integer_init(32);
    List *list = list_init(TYPE_INTEGER);
    memory_summary();
    printf("Its size is : %d / 0\n", list_size(list));
    value_print_message("Adding first element %v\n", a);
    list_append(list, a);
    value_print_message("Adding second element %v\n", b);
    list_append(list, b);
    value_print_message("Adding third element %v\n", c);
    list_append(list, c);
    printf("Its size is : %d / 3\n", list_size(list));
    list_print(list);
    printf("Replacing value at 1\n");
    Value d = integer_init(24);
    list_set(list, 1, d);
    list_print(list);
    printf("Freeing memory\n");
    list_free(list);
    memory_summary();
    printf("End of test_list()\n");
}

void test_list_str()
{
    printf("\n--------------------------------------------------\n");
    printf("Start of test_list_str()\n");
    printf("--------------------------------------------------\n");
    memory_summary();
    printf("Creating objets\n");
    Value a = string_init("bonjour");
    Value b = string_init("le");
    Value c = string_init("monde");
    List *list = list_init(TYPE_STRING);
    memory_summary();
    printf("Its size is : %d / 0\n", list_size(list));
    value_print_message("Adding first element %v\n", a);
    list_append(list, a);
    value_print_message("Adding second element %v\n", b);
    list_append(list, b);
    value_print_message("Adding third element %v\n", c);
    list_append(list, c);
    printf("Its size is : %d / 3\n", list_size(list));
    list_print(list);
    printf("Replacing value at 1\n");
    Value d = string_init("du");;
    list_set(list, 1, d);
    list_print(list);
    printf("Replaced value is:\n");
    value_print(b);
    printf("\n");
    printf("Freeing memory\n");
    list_free(list);
    string_delete(a);
    string_delete(b);
    string_delete(c);
    string_delete(d);
    memory_summary();
    printf("End of test_list_str()\n");
}

void test_dict()
{
    printf("Start of test_dict()\n");
    memory_summary();
    printf("Creating objets for a int => bool dict\n");
    Value k1 = integer_init(5);
    Value v1 = boolean_init(true);
    Value k2 = integer_init(10);
    Value v2 = boolean_init(false);
    Value k3 = integer_init(32);
    Value v3 = boolean_init(true);
    Dict * dict = dict_init(TYPE_INTEGER, TYPE_BOOLEAN);
    memory_summary();
    printf("Its size is : %d / 0\n", dict_size(dict));
    printf("Adding first element\n");
    dict_set(dict, k1, v1);
    printf("Adding second element\n");
    dict_set(dict, k2, v2);
    printf("Adding third element\n");
    dict_set(dict, k3, v3);
    printf("Its size is : %d / 3\n", dict_size(dict));
    dict_print(dict);
    printf("Replacing value at key 10 by true");
    Value v4 = boolean_init(true);
    dict_set(dict, k2, v4);
    dict_print(dict);
    printf("Freeing memory\n");
    dict_free(dict);
    memory_summary();
    printf("End of test_dict()\n");
}

void test_dict_str()
{
    printf("\n--------------------------------------------------\n");
    printf("Start of test_dict_str()\n");
    printf("--------------------------------------------------\n");
    memory_summary();
    printf("Creating objets for a str => int dict\n");
    Value k1 = string_init("bonjour");
    Value k2 = string_init("le");
    Value k3 = string_init("monde");
    Value v1 = integer_init(10);
    Value v2 = integer_init(50);
    Value v3 = integer_init(100);
    Dict * dict = dict_init(TYPE_STRING, TYPE_INTEGER);
    memory_summary();
    printf("Its size is : %d / 0\n", dict_size(dict));
    printf("Adding first element\n");
    dict_set(dict, k1, v1);
    printf("Adding second element\n");
    dict_set(dict, k2, v2);
    printf("Adding third element\n");
    dict_set(dict, k3, v3);
    printf("Its size is : %d / 3\n", dict_size(dict));
    dict_print(dict);
    printf("Replacing value at key 'le' by 99\n");
    Value v4 = integer_init(99);
    dict_set(dict, k2, v4);
    dict_print(dict);
    printf("Replaced value is:\n");
    value_print(k2);
    printf("\n");
    printf("Freeing memory\n");
    dict_free(dict);
    memory_summary();
    printf("End of test_dict()\n");
}

const int TEST = 1;

int main(int argc, char *argv[])
{
    printf("Running with %d parameters\n", argc);
    for (int i = 0; i < argc; i++)
    {
        printf("%d. %s\n", i, argv[i]);
    }

    if (TEST == 1)
    {
        //test_list();
        test_list_str();
        //test_dict();
        test_dict_str();
    }
    else if (TEST == 2)
    {
        /*
        printf("Test de Token:\n");
        char *text = "a + 5";
        Token t1 = {.start = 0, .count = 1, .text = text, .type = TOKEN_IDENTIFIER};
        Token t2 = {.start = 2, .count = 1, .text = text, .type = TOKEN_OPERATOR};
        Token t3 = {.start = 4, .count = 1, .text = text, .type = TOKEN_DECIMAL};
        token_print(t1);
        token_print(t2);
        token_print(t3);

        printf("Test d'une TokenList:\n");
        TokenList *tkl = token_list_init();
        token_list_append(tkl, t1);
        token_list_append(tkl, t2);
        token_list_append(tkl, t3);
        token_list_print(tkl);

        while (token_list_size(tkl) > 0)
        {
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
        */
    }

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
