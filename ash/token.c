#include "token.h"

//-----------------------------------------------------------------------------
// Constantes
//-----------------------------------------------------------------------------

const char *TOKEN_TYPE_TO_STRING[] = {
    "NONE",
    "DECIMAL",
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
    "STRING",
    "NIL",
    "COMMENT",
    "ERROR_LONELY_OPERATOR",
    "ERROR_NUMBER_WITH_LETTER"};
const uint8_t TOKEN_TYPE_TO_STRING_MAX_LENGTH = 24;
const char *HEXADECIMAL_ELEMENTS = "0123456789abcdefABCDEF";
const char *OPERATOR_ELEMENTS = "+-*/%<>=!.~?&|^";
const char *SEPARATOR_ELEMENTS = "([{}]),;:\t";

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
    "not",
    "true",
    "false",
    "nil"};

//-----------------------------------------------------------------------------
// Token Functions
//-----------------------------------------------------------------------------

char *token_value(Token tok)
{
    char *s = (char *)memory_get(tok.count + 1);
    memset(s, '\0', tok.count + 1);
    for (unsigned int i = 0; i < tok.count; i++)
    {
        s[i] = tok.text[tok.start + i];
    }
    return s;
}

void token_print(Token tok)
{
    printf("{%s @%d #%d L.%03d |%.*s|}", TOKEN_TYPE_TO_STRING[tok.type], tok.start, tok.count, tok.line, tok.count, tok.text + tok.start);
}

void token_print_value(Token tok)
{
    printf("%.*s", tok.count, tok.text + tok.start);
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

bool token_is_nil(Token t)
{
    return token_cmp(t, "nil");
}

bool token_is_boolean(Token t)
{
    return token_cmp(t, "true") || token_cmp(t, "false");
}

bool token_is_operator(Token t)
{
    return token_cmp(t, "and") || token_cmp(t, "or") || token_cmp(t, "not");
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

bool token_eq(Token t1, Token t2)
{
    return t1.count == t2.count && t1.line == t2.line && t1.start == t2.start && t1.text == t2.text && t1.type == t2.type;
}

//-----------------------------------------------------------------------------
// TokenDynArray Functions
//-----------------------------------------------------------------------------

TokenDynArray token_dyn_array_init(void)
{
    TokenDynArray tda;
    tda.capacity = 8;
    tda.count = 0;
    tda.data = malloc(sizeof(Token) * tda.capacity);
    return tda;
}

void token_dyn_array_free(TokenDynArray * tda)
{
    tda->count = 0;
    tda->capacity = 0;
    free(tda->data);
}

uint32_t token_dyn_array_add(TokenDynArray * tda, Token t)
{
    if (tda->count + 1 == tda->capacity)
    {
        tda->capacity = tda->capacity * 2;
        tda->data = realloc(tda->data, sizeof(Token) * tda->capacity);
        if (tda->data == NULL)
        {
            general_message(FATAL, "Out of memory when reallocating TokenDynArray.");
        }
    }
    tda->data[tda->count] = t;
    tda->count += 1;
    return tda->capacity - tda->count;
}

Token token_dyn_array_get(TokenDynArray tda, int32_t index)
{
    if (index >= 0)
    {
        return tda.data[index];
    }
    else
    {
        return tda.data[tda.count + index];
    }
}

void token_dyn_array_info(TokenDynArray tda)
{
    printf("TokenDynArray @%p data@%p data#%u %u/%u (size of Token = %u)\n", &tda, tda.data, _msize(tda.data), tda.count, tda.capacity, sizeof(Token));
    uint32_t size = token_dyn_array_size(tda);
    for (uint32_t i = 0; i < size; i++)
    {
        printf("    %03d.", i);
        token_print(token_dyn_array_get(tda, i));
        printf("\n");
    }
}

uint32_t token_dyn_array_size(TokenDynArray tda)
{
    return tda.count;
}
