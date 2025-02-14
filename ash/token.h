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
// Mots : const if then elsif else end while do loop for in next break fun pro return class and or not

#define NB_KEYWORDS 20

// Liste des caractères qui composent les opérateurs ou les hexadécimaux

// Opérateurs mathématiques  : + - * / ** // %
// Opérateurs de comparaison : < <= > >= !=
// Opérateur d'affectation   : =
// Opérateur de portée       : .
// Opérateur d'héritage      : <
// Opérateur d'intervalle    : ..
// Opérateur de décalage     : << >>
// Caractères : + - * / % < > = ! .

// Séparateurs              : ( ) [ ] { } , :
// Caractères : ( ) [ ] { } , :

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void token_print(Token tok);
bool token_cmp(Token t, const char *s);
bool token_is_boolean(Token t);
bool token_is_keyword(Token t);

#endif
