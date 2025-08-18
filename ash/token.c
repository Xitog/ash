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

const char *NIL = "nil";

const char *BOOLEANS[] = {
    "true",
    "false"};

const char *OPERATORS[] = {
    "and",
    "or",
    "not"};

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
    "class"};

//-----------------------------------------------------------------------------
// TextPart Functions
//-----------------------------------------------------------------------------

TextPart text_part_init(const char * text, uint32_t start, uint32_t length)
{
    TextPart tp;
    tp.source = text;
    tp.start = start;
    tp.length = length;
    return tp;
}

bool text_part_cmp(TextPart tp, const char *to)
{
    if (strlen(to) != tp.length)
    {
        return false;
    }
    uint32_t i = 0;
    while (i < tp.length)
    {
        if (tp.source[tp.start + i] != to[i])
        {
            return false;
        }
        i += 1;
    }
    return true;
}

bool text_part_cmps(TextPart tp, const char *array_to[], uint32_t size)
{
    // Il faut parcourir les chaînes du tableau et tester si on reconnaît l'un d'entre elles
    for (uint32_t i = 0; i < size; i++)
    {
        if (strlen(array_to[i]) == tp.length)
        {
            if (text_part_cmp(tp, array_to[i]))
            {
                return true;
            }
        }
    }
    return false;
}

void text_part_print(TextPart tp)
{
    printf("%.*s", tp.length, tp.source + tp.start);
}

bool text_part_eq(TextPart tp1, TextPart tp2)
{
    return tp1.source == tp2.source && tp1.start == tp2.start && tp1.length == tp2.length;
}

//-----------------------------------------------------------------------------
// Token Functions
//-----------------------------------------------------------------------------

Token token_init(TextPart text, uint32_t line, TokenType type)
{
    Token t;
    t.text = text;
    t.line = line;
    t.type = type;
    return t;
}

char *token_value(Token tok)
{
    char *s = (char *)memory_get(tok.text.length + 1);
    memset(s, '\0', tok.text.length + 1);
    for (unsigned int i = 0; i < tok.text.length; i++)
    {
        s[i] = tok.text.source[tok.text.start + i];
    }
    return s;
}

void token_print(Token tok)
{
    // Using TOKEN_TYPE_TO_STRING_MAX_LENGTH value
    if (tok.type != TOKEN_NEWLINE)
    {
        printf("{%-24s @%d #%d L.%03d |%.*s|}", TOKEN_TYPE_TO_STRING[tok.type], tok.text.start, tok.text.length, tok.line, tok.text.length, tok.text.source + tok.text.start);
    }
    else
    {
        printf("{%-24s @%d #%d L.%03d |<NEWLINE>|}", TOKEN_TYPE_TO_STRING[tok.type], tok.text.start, tok.text.length, tok.line);
    }
}

void token_print_text(Token tok)
{
    text_part_print(tok.text);
}

bool token_eq(Token t1, Token t2)
{
    return text_part_eq(t1.text, t2.text) && t1.line == t2.line && t1.type == t2.type;
}
