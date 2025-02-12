#ifndef __TOKEN_LIST__
#define __TOKEN_LIST__

//-----------------------------------------------------------------------------
// Imports
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "general.h"
#include "token.h"

//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------

typedef struct _TokenListElement
{
    Token token;
    struct _TokenListElement *next;
    struct _TokenListElement *prev;
} TokenListElement;

typedef struct _TokenList
{
    TokenListElement *head;
    TokenListElement *tail;
    uint32_t count;
} TokenList;

//-----------------------------------------------------------------------------
// Fonctions
//-----------------------------------------------------------------------------

TokenList *token_list_init();
uint32_t token_list_append(TokenList *list, Token token);
Token token_list_get(TokenList *list, uint32_t index);
Token token_list_pop(TokenList *list);
void token_list_free(TokenList *list);
uint32_t token_list_size(TokenList *list);
bool token_list_is_empty(TokenList *list);
void token_list_reverse(TokenList *list);
void token_list_print(TokenList *list);
void token_list_free(TokenList *list);

#endif
