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
    WRONG_TOKEN_LONELY_OPERATOR = 14,
    WRONG_TOKEN_NUMBER_AND_LETTER = 15
} TokenType;

#define NB_TOKEN_TYPES 16

typedef struct _Token
{
    const char * text;
    TokenType type;
    uint32_t start;
    uint32_t count;
} Token;

//-----------------------------------------------------------------------------
// Constantes
//-----------------------------------------------------------------------------

const extern char *TOKEN_TYPE_REPR_STRING[];
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

void token_print(Token tok);
bool token_cmp(Token t, const char *s);
bool token_is_nil(Token t);
bool token_is_boolean(Token t);
bool token_is_operator(Token t);
bool token_is_keyword(Token t);

#endif
