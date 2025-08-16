// Test of DynArray

// Compile : cl /W4 .\tests_dyn_array.c .\token.c .\general.c /Fe:tests_dyn_array.exe
// Clean   : del *.obj

#include "general.h"

void display_char(void *c)
{
    char *pc = c;
    printf("%c @%p", *pc, pc);
}

bool sup_char(void *c1, void *c2)
{
    char *pc1 = c1;
    char *pc2 = c2;
    return c1 > c2;
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

    printf("= Start of test 1 : creating a dyanmic array\n");
    DynArray da1 = dyn_array_init(sizeof(char));
    char *c1 = malloc(sizeof(char));
    *c1 = 'e';
    char *c2 = malloc(sizeof(char));
    *c2 = 'h';
    char *c3 = malloc(sizeof(char));
    *c3 = 'z';
    char *c4 = malloc(sizeof(char));
    *c4 = 'a';
    printf("c1  e = %c @%p\n", *c1, c1);
    printf("c2  h = %c @%p\n", *c2, c2);
    printf("c3  z = %c @%p\n", *c3, c3);
    printf("c4  a = %c @%p\n", *c4, c4);
    dyn_array_info(da1, display_char);
    dyn_array_add(&da1, c1);
    dyn_array_add(&da1, c2);
    dyn_array_add(&da1, c3);
    dyn_array_add(&da1, c4);
    dyn_array_info(da1, display_char);

    DynArray da1_sorted = dyn_array_init(sizeof(char));

}
