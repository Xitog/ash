//-----------------------------------------------------------------------------
// Directives
//-----------------------------------------------------------------------------

#define DEBUG

#ifndef __PARSER__
#define __PARSER__

#include <stdint.h>
#include "token.h"
#include "token_list.h"

typedef enum _NodeType
{
    NODE_INTEGER = 0,
    NODE_OPERATOR_MUL = 1,
    NODE_OPERATOR_ADD = 2
} NodeType;

typedef struct _Node
{
    struct _Node *left;
    //struct _Node *middle;
    struct _Node *right;
    Token token;
    NodeType type;
} Node;

typedef struct
{
    Node *root;
} Tree;

extern unsigned int parser_index;

bool check_type(TokenList *list, uint32_t index, TokenType expected);
bool check_value(TokenList *list, uint32_t index, TokenType expected_type, const char *expected_value);

Tree *parse(TokenList *list);
Node *parse_expression(TokenList *list);
Node *parse_addition(TokenList *list);
Node *parse_multiplication(TokenList *list);
Node *parse_litteral(TokenList *list);

void ast_print(Tree *tree);

#endif
