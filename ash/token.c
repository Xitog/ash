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
