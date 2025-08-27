// Test of DynArray

// Compile : cl /W4 .\tests_dyn_array.c .\token.c .\general.c /Fe:tests_dyn_array.exe
// Clean   : del *.obj

#include "..\general.h"

void display_char(void *c)
{
    char *pc = c;
    printf("%c @%p", *pc, pc);
}

bool sup_char(void *c1, void *c2)
{
    char *pc1 = c1;
    char *pc2 = c2;
    return *pc1 > *pc2;
}

int main(int argc, char *argv[])
{
    set_display_error(EL_DEBUG);

    if (argc > 2)
    {
        printf("Arguments:\n");
        for (int i = 0; i < argc; i++)
        {
            printf("%4d. %s\n", i, argv[i]);
        }
    }

    char *c1 = malloc(sizeof(char));
    *c1 = 'e';
    char *c2 = malloc(sizeof(char));
    *c2 = 'h';
    char *c3 = malloc(sizeof(char));
    *c3 = 'z';
    char *c4 = malloc(sizeof(char));
    *c4 = 'a';
    char *c5 = malloc(sizeof(char));
    *c5 = 'b';
    printf("c1  e = %c @%p\n", *c1, c1);
    printf("c2  h = %c @%p\n", *c2, c2);
    printf("c3  z = %c @%p\n", *c3, c3);
    printf("c4  a = %c @%p\n", *c4, c4);
    printf("c5  b = %c @%p\n", *c5, c5);

    printf("\n= Start of test 1 : creating a dynamic array\n");
    DynArray da1 = dyn_array_init(sizeof(char));
    dyn_array_info(da1, display_char);
    dyn_array_append(&da1, c1);
    dyn_array_append(&da1, c2);
    dyn_array_append(&da1, c3);
    dyn_array_append(&da1, c4);
    dyn_array_append(&da1, c5);
    dyn_array_info(da1, display_char);

    printf("\n= Start of test 2 : deleting an element at start, in the middle, at the end\n");
    DynArray da2 = dyn_array_init(sizeof(char));
    dyn_array_append(&da2, c1);
    dyn_array_append(&da2, c2);
    dyn_array_append(&da2, c3);
    dyn_array_append(&da2, c4);
    dyn_array_append(&da2, c5);
    printf("Should be: (#5) e h z a b\n");
    dyn_array_info(da2, display_char);
    dyn_array_delete(&da2, 0);
    printf("Should be: (#4) h z a b\n");
    dyn_array_info(da2, display_char);
    dyn_array_delete(&da2, 1);
    printf("Should be: (#3) h a b\n");
    dyn_array_info(da2, display_char);
    dyn_array_delete(&da2, 2);
    printf("Should be: (#2) h a\n");
    dyn_array_info(da2, display_char);

    printf("\n= Start of test 3 : insert an element at start (empty), at start (not empty), in the middle, at the end\n");
    DynArray da3 = dyn_array_init(sizeof(char));
    dyn_array_insert(&da3, c1, 0);
    printf("Should be: (#1) e\n");
    dyn_array_info(da3, display_char);
    dyn_array_insert(&da3, c4, 0);
    printf("Should be: (#2) a e\n");
    dyn_array_info(da3, display_char);
    dyn_array_insert(&da3, c5, 1);
    printf("Should be: (#3) a b e\n");
    dyn_array_info(da3, display_char);
    dyn_array_insert(&da3, c2, 3);
    printf("Should be: (#4) a b e h\n");
    dyn_array_info(da3, display_char);

    printf("\n= Start of test 4 : append sorted\n");
    DynArray da4 = dyn_array_init(sizeof(char));
    dyn_array_append_sorted(&da4, c1, sup_char);
    dyn_array_append_sorted(&da4, c2, sup_char);
    dyn_array_append_sorted(&da4, c3, sup_char);
    dyn_array_append_sorted(&da4, c4, sup_char);
    dyn_array_append_sorted(&da4, c5, sup_char);
    printf("Should be: (#5) a b e h z\n");
    dyn_array_info(da4, display_char);
}
