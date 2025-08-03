// Test of Token, TokenDynArray, lex

// Compile with :
// cl /W4 .\tests_tokens.c .\token.c .\lexer.c .\general.c /Fe:tests_tokens.exe

#include "token.h"
#include "lexer.h"

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

    printf("= Start of test 1 : getting a dyanmic array of tokens from lex()\n");
    char * text1 = "if a == 5 then\n hello()\n end\n";
    printf("Lexing :\n%s", text1);
    TokenDynArray tda1 = lex(text1, true, false);
    token_dyn_array_info(tda1);
    printf("= End of test 1\n");

    printf("= Start of test 2 : access and modification of the dynamic array of tokens of test 1\n");
    //token_dyn_array_get(tda1, 12);
    printf("Getting token at index -12\n");
    Token t2 = token_dyn_array_get(tda1, -12);
    general_message(EL_DEBUG, "   -12. %t", t2);
    //t2 = token_dyn_array_get(tda1, -13);
    printf("Deleting token at index 5\n");
    token_dyn_array_delete(&tda1, 5);
    token_dyn_array_info(tda1);
    printf("= End of test 2\n");

}
