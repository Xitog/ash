#include "token.h"

//-----------------------------------------------------------------------------
// Constantes
//-----------------------------------------------------------------------------

const char *TYPE_REPR_STRING[] = {
    "NONE",
    "INTEGER",
    "HEXADECIMAL",
    "BINARY",
    "FLOAT",
    "BOOLEAN",
    "IDENTIFIER",
    "KEYWORD",
    "SPACE",
    "OPERATOR",
    "NEWLINE",
    "SEPARATOR",
    "STRING"};

const char *HEXADECIMAL_ELEMENTS = "0123456789abcdefABCDEF";
const char *OPERATOR_ELEMENTS = "+-*/%<>=!.";
const char *SEPARATOR_ELEMENTS = "([{}]),:\n\t";

const char *KEYWORDS[] = {
    "const",
    "if",
    "then",
    "else",
    "elsif",
    "end",
    "while",
    "do",
    "loop",
    "for",
    "in",
    "next",
    "break",
    "fun",
    "pro",
    "return",
    "class",
    "and",
    "or",
    "not"};

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void token_print(Token tok)
{
    printf("{%s @%d #%d |%.*s|}\n", TYPE_REPR_STRING[tok.type], tok.start, tok.count, tok.count, tok.text + tok.start);
}

bool token_cmp(Token t, const char *str)
{
    if (strlen(str) != t.count)
    {
        return false;
    }
    unsigned int i = 0;
    while (i < t.count)
    {
        if (t.text[t.start + i] != str[i])
        {
            return false;
        }
        i += 1;
    }
    return true;
}

bool token_is_boolean(Token t)
{
    return token_cmp(t, "true") || token_cmp(t, "false");
}

bool token_is_keyword(Token t)
{
    // Il faut parcourir les keywords et tester si on reconnaît l'un d'entre eux
    for (int i = 0; i < NB_KEYWORDS; i++)
    {
        if (strlen(KEYWORDS[i]) == t.count)
        {
            if (token_cmp(t, KEYWORDS[i]))
            {
                return true;
            }
        }
    }
    return false;
}
