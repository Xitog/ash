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

extern uint8_t DEBUG_MODE;
extern DynArray root_scope;

typedef enum _NodeType
{
    NODE_COMMENT = 0,
    NODE_INTEGER = 1,
    NODE_FLOAT = 2,
    NODE_BOOLEAN = 3,
    NODE_STRING = 4,
    NODE_IDENTIFIER = 5,
    NODE_BINARY_OPERATOR = 6,
    NODE_IF = 7,
    NODE_WHILE = 8,
    NODE_FUNCTION_CALL = 9,
    NODE_BLOCK = 10
} NodeType;

typedef struct _Node
{
    struct _Node *left;
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

typedef struct _Variable
{
    TextPart text;
    ValueType type;
} Variable;

Node *node_init(NodeType nt);
bool node_is_type(Node * node, NodeType type);
void node_print(Node *node);
void node_print_level(Node *node, uint32_t level);

extern unsigned int parser_index;
const extern char *NODE_TYPE_REPR_STRING[];

bool check_token_type(DynArray list, uint32_t index, TokenType expected);
bool check_token_value(DynArray list, uint32_t index, TokenType expected_type, const char *expected_value);

bool parser_set_debug(bool debug);
bool parser_get_debug();

void parser_init();
AST *parse(DynArray list);

Node *parse_block(DynArray list);
Node *parse_if(DynArray list);
Node *parse_while(DynArray list);
Node *parse_expression(DynArray list);
Node *parse_combined_affectation_binary(DynArray list);
Node *parse_combined_affectation_shift(DynArray list);
Node *parse_combined_affectation(DynArray list);
Node *parse_affectation(DynArray list);
Node *parse_interval(DynArray list);
Node *parse_logical_or(DynArray list);
Node *parse_logical_and(DynArray list);
Node *parse_equality(DynArray list);
Node *parse_comparison(DynArray list);
Node *parse_binary_or_xor(DynArray list);
Node *parse_binary_and(DynArray list);
Node *parse_shift(DynArray list);
Node *parse_addition_soustraction(DynArray list);
Node *parse_multiplication_division_modulo(DynArray list);
Node *parse_unary_minus(DynArray list);
Node *parse_pow(DynArray list);
Node *parse_unary_complement(DynArray list);
// [] .
Node *parse_call(DynArray list);
Node *parse_identifier_left_aff(DynArray list, ValueType type_to_set);
Node *parse_parenthesis_expr(DynArray list);
Node *parse_litteral(DynArray list);

NodeType node_compute_type(Node *node);

void ast_print(AST *tree);
void ast_to_dot(AST *tree, const char * res);

void print_root_scope();

#endif
