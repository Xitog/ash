#ifndef __TOKEN__
#define __TOKEN__

//-----------------------------------------------------------------------------
// Imports
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdio.h>

//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------

typedef enum _Type
{
    TOKEN_NONE = 0,
    TOKEN_DECIMAL = 1,
    TOKEN_HEXADECIMAL = 2,
    TOKEN_BINARY = 3,
    TOKEN_FLOAT = 4,
    TOKEN_BOOLEAN = 5,
    TOKEN_IDENTIFIER = 6,
    TOKEN_KEYWORD = 7,
    TOKEN_SPACE = 8,
    TOKEN_OPERATOR = 9,
    TOKEN_NEWLINE = 10,
    TOKEN_SEPARATOR = 11,
    TOKEN_STRING = 12
} Type;

typedef struct _Token
{
    const char * text;
    Type type;
    uint32_t start;
    uint32_t count;
} Token;

//-----------------------------------------------------------------------------
// Constantes
//-----------------------------------------------------------------------------

const extern char *TYPE_REPR_STRING[];

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void token_print(Token tok);

#endif
