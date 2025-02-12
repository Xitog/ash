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

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void token_print(Token tok)
{
    printf("{%s @%d #%d |%.*s|}\n", TYPE_REPR_STRING[tok.type], tok.start, tok.count, tok.count, tok.text + tok.start);
}
