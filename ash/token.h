#ifndef __TOKEN__
#define __TOKEN__

//-----------------------------------------------------------------------------
// Imports
//-----------------------------------------------------------------------------

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "general.h"

//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------

typedef enum _TokenType
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
    TOKEN_STRING = 12,
    TOKEN_NIL = 13,
    TOKEN_COMMENT = 14,
    TOKEN_ERROR_LONELY_OPERATOR = 15,
    TOKEN_ERROR_NUMBER_WITH_LETTER = 16
} TokenType;

typedef struct _Token
{
    const char *text;
    TokenType type;
    uint32_t start;
    uint32_t count;
    uint32_t line;
} Token;

typedef struct _TokenDynArray
{
    Token * data;
    uint32_t count;
    uint32_t capacity;
} TokenDynArray;

//-----------------------------------------------------------------------------
// Constantes
//-----------------------------------------------------------------------------

const extern char *TOKEN_TYPE_TO_STRING[];
const extern uint8_t TOKEN_TYPE_TO_STRING_MAX_LENGTH;

const extern char *OPERATOR_ELEMENTS;
const extern char *HEXADECIMAL_ELEMENTS;
const extern char *SEPARATOR_ELEMENTS;
const extern char *KEYWORDS[];

// Liste des keywords

// Const, var  : const
// Sélection   : if / then / elsif / else / end
// Itération   : while / do / loop / for / in / next / break
// Sous-prog   : fun / pro / end / return
// Classes     : class end
// Opérateurs  : and or not
// Valeurs     : true false nil
// 23 Mots : const if then elsif else end while do loop for in next break fun pro return class and or not true false nil

#define NB_KEYWORDS 20

// Liste des caractères qui composent les opérateurs ou les hexadécimaux

// Opérateurs binaires
//  7 Opérateurs mathématiques  : + -  * /  ** // %
//  6 Opérateurs de comparaison : < <= > >= == !=
//  1 Opérateur d'affectation   : =
// 14 Opérateurs d'aff combinés : += -= *= /= **= //= %= &= |= ^= <<= >>= ~= ??=
//  1 Opérateur de portée       : .
//  1 Opérateur d'héritage      : <
//  1 Opérateur d'intervalle    : ..
//  5 Opérateurs binaires       : & | ^ << >>
//  1 Opérateur de fusion nil   : ??
// Caractères : + - * / % < > = ! . ~ ? & | ^

// Opérateurs unaires
//  1 Opérateur mathématique    : -
//  1 Opérateur binaire         : ~

//  8 Séparateurs               : ( ) [ ] { } , :
// Caractères : ( ) [ ] { } , :

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

char *token_value(Token tok);
void token_print(Token tok);
void token_print_value(Token tok);
bool token_cmp(Token t, const char *s);
bool token_is_nil(Token t);
bool token_is_boolean(Token t);
bool token_is_operator(Token t);
bool token_is_keyword(Token t);
bool token_eq(Token t1, Token t2);

TokenDynArray token_dyn_array_init();
uint32_t token_dyn_array_add(TokenDynArray * tda, Token t);
Token token_dyn_array_get(TokenDynArray tda, int32_t index);
void token_dyn_array_info(TokenDynArray tda);

#endif
