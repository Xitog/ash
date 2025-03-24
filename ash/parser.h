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
    NODE_BINARY_OPERATOR = 1
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
Node *parse_combined_affectation_binary(TokenList *list);
Node *parse_combined_affectation_shift(TokenList *list);
Node *parse_combined_affectation(TokenList *list);
Node *parse_affectation(TokenList *list);
Node *parse_interval(TokenList *list);
Node *parse_logical_or(TokenList *list);
Node *parse_logical_and(TokenList *list);
Node *parse_equality(TokenList *list);
Node *parse_comparison(TokenList *list);
Node *parse_binary_or_xor(TokenList *list);
Node *parse_binary_and(TokenList *list);
Node *parse_shift(TokenList *list);
Node *parse_addition_soustraction(TokenList *list);
Node *parse_multiplication_division_modulo(TokenList *list);
Node *parse_unary_minus(TokenList *list);
Node *parse_pow(TokenList *list);
Node *parse_unary_complement(TokenList *list);
// () [] .
Node *parse_litteral(TokenList *list);

void ast_print(Tree *tree);

#endif
