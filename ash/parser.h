//-----------------------------------------------------------------------------
// Directives
//-----------------------------------------------------------------------------

#define DEBUG_MODE_FULL 2
#define DEBUG_MODE_CLEVER 1

#ifndef __PARSER__
#define __PARSER__

#include <stdint.h>
#include "token.h"
#include "value.h"
#include "dict.h"

extern uint8_t DEBUG_MODE;
extern Dict * root_scope;

typedef enum _NodeType
{
    NODE_INTEGER = 0,
    NODE_FLOAT = 1,
    NODE_BOOLEAN = 2,
    NODE_STRING = 3,
    NODE_IDENTIFIER = 4,
    NODE_BINARY_OPERATOR = 5,
    NODE_IF = 6,
    NODE_WHILE = 7,
    NODE_FUNCTION_CALL = 8,
    NODE_BLOCK = 9
} NodeType;

typedef struct _Node
{
    struct _Node *left;
    //struct _Node *middle;
    struct _Node *right;
    struct _Node *extra; // for if condition
    Token token;
    NodeType type;
    ValueType value_type;
} Node;

typedef struct
{
    Node *root;
} AST;

bool node_is_type(Node * node, NodeType type);
void node_print(Node *node);
void node_print_level(Node *node, uint32_t level);

extern unsigned int parser_index;
const extern char *NODE_TYPE_REPR_STRING[];

bool check_token_type(TokenDynArray list, uint32_t index, TokenType expected);
bool check_token_value(TokenDynArray list, uint32_t index, TokenType expected_type, const char *expected_value);

bool parser_set_debug(bool debug);
bool parser_get_debug();

AST *parse(TokenDynArray list);

Node *parse_block(TokenDynArray list);
Node *parse_if(TokenDynArray list);
Node *parse_while(TokenDynArray list);
Node *parse_expression(TokenDynArray list);
Node *parse_combined_affectation_binary(TokenDynArray list);
Node *parse_combined_affectation_shift(TokenDynArray list);
Node *parse_combined_affectation(TokenDynArray list);
Node *parse_affectation(TokenDynArray list);
Node *parse_interval(TokenDynArray list);
Node *parse_logical_or(TokenDynArray list);
Node *parse_logical_and(TokenDynArray list);
Node *parse_equality(TokenDynArray list);
Node *parse_comparison(TokenDynArray list);
Node *parse_binary_or_xor(TokenDynArray list);
Node *parse_binary_and(TokenDynArray list);
Node *parse_shift(TokenDynArray list);
Node *parse_addition_soustraction(TokenDynArray list);
Node *parse_multiplication_division_modulo(TokenDynArray list);
Node *parse_unary_minus(TokenDynArray list);
Node *parse_pow(TokenDynArray list);
Node *parse_unary_complement(TokenDynArray list);
// [] .
Node *parse_call(TokenDynArray list);
Node *parse_identifier_left_aff(TokenDynArray list, ValueType type_to_set);
Node *parse_parenthesis_expr(TokenDynArray list);
Node *parse_litteral(TokenDynArray list);

NodeType node_compute_type(Node *node);

void ast_print(AST *tree);
void ast_to_dot(AST *tree, const char * res);

#endif
