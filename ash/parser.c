#include "parser.h"

#define SPACES 4

uint8_t DEBUG_MODE = DEBUG_MODE_FULL; // DEBUG_MODE_CLEVER;

unsigned int parser_index = 0;
unsigned int parser_level = 0;

bool node_is_type(Node *node, NodeType type)
{
    return node->type == type;
}

bool check_token_type(TokenList *list, uint32_t index, TokenType expected)
{
    if (list == NULL)
    {
        return false;
    }
    else if (index >= token_list_size(list))
    {
        return false;
    }
    Token t = token_list_get(list, index);
    return t.type == expected;
}

bool check_token_value(TokenList *list, uint32_t index, TokenType expected_type, const char *expected_value)
{
    if (list == NULL)
    {
        return false;
    }
    else if (index >= token_list_size(list))
    {
        return false;
    }
    Token t = token_list_get(list, index);
    // printf("%d vs expected: %d\n", t.type, expected_type);
    // printf("expected: %s\n", expected_value);
    // token_print(t);
    return t.type == expected_type && token_cmp(t, expected_value);
}

void tab()
{
    for (unsigned int i = 0; i < parser_level * SPACES; i++)
    {
        if (i % SPACES == 0)
        {
            printf("|");
        }
        else
        {
            printf(" ");
        }
    }
}

Tree *parse(TokenList *list)
{
    parser_index = 0;
    parser_level = 0;
#ifdef DEBUG
    printf("> parse at %d\n", parser_index);
#endif
    Tree *t = (Tree *)memory_get(sizeof(Tree));
    t->root = parse_block(list);
#ifdef DEBUG
    printf("end of parsing at %d / %d\n", parser_index, token_list_size(list));
#endif
    return t;
}

Node *parse_block(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    tab();
    printf("> parse_block at %d\n", parser_index);
#endif
    Node *first = NULL;
    Node *node = NULL;
    Node *next = NULL;
    do
    {
        while (check_token_value(list, parser_index, TOKEN_SEPARATOR, ";"))
        {
            parser_index += 1;
        }
        if (check_token_value(list, parser_index, TOKEN_KEYWORD, "if"))
        {
            next = parse_if(list);
        }
        else if (parser_index < token_list_size(list))
        {
            next = parse_expression(list);
        }
        if (next != NULL && node == NULL)
        {
            node = next;
            first = node;
        }
        else if (next != NULL && node != NULL)
        {
            if (node_is_type(node, NODE_BLOCK))
            {
                Node *block = (Node *)memory_get(sizeof(Node));
                block->type = NODE_BLOCK;
                block->token = node->right->token;
                block->left = node->right;
                block->right = next;
                node->right = block;
                node = block;
            }
            else
            {
                Node *block = (Node *)memory_get(sizeof(Node));
                block->type = NODE_BLOCK;
                block->token = node->token;
                block->left = node;
                block->right = next;
                node = block;
                first = node;
            }
        }
        next = NULL;
    } while (check_token_value(list, parser_index, TOKEN_SEPARATOR, ";") && parser_index < token_list_size(list));
    return first;
}

Node *parse_if(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    tab();
    printf("> parse_if at %d\n", parser_index);
#endif
    Node *node = node = (Node *)memory_get(sizeof(Node));
    Token t = token_list_get(list, parser_index);
    node->token = t;   // keyword if
    parser_index += 1; // pass keyword if
    Node *condition = parse_expression(list);
    if (!check_token_value(list, parser_index, TOKEN_KEYWORD, "then"))
    {
        general_error("if not followed by then.");
    }
#ifdef DEBUG
    tab();
    printf("> parse THEN\n");
#endif
    parser_index += 1; // pass keyword then
    Node *action = parse_block(list);
    if (!check_token_value(list, parser_index, TOKEN_KEYWORD, "end"))
    {
        general_error("if not terminated by end.");
    }
#ifdef DEBUG
    tab();
    printf("> parse END\n");
#endif
    parser_index += 1; // pass keyword end
    node->extra = condition;
    node->left = action;
    node->right = NULL;
    node->type = NODE_IF;
    parser_level--;
    return node;
}

Node *parse_expression(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    if (DEBUG_MODE == DEBUG_MODE_FULL)
    {
        tab();
        printf("> parse_expression at %d\n", parser_index);
    }
#endif
    Node *n = parse_combined_affectation_binary(list);
    parser_level--;
    return n;
}

Node *parse_combined_affectation_binary(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    if (DEBUG_MODE == DEBUG_MODE_FULL)
    {
        tab();
        printf("? parse_combined_affectation_binary at %d\n", parser_index);
    }
#endif
    Node *n = parse_combined_affectation_shift(list);
    parser_level--;
    return n;
}

Node *parse_combined_affectation_shift(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    if (DEBUG_MODE == DEBUG_MODE_FULL)
    {
        tab();
        printf("? parse_combined_affectation_shift at %d\n", parser_index);
    }
#endif
    Node *n = parse_combined_affectation(list);
    parser_level--;
    return n;
}

Node *parse_combined_affectation(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    if (DEBUG_MODE == DEBUG_MODE_FULL)
    {
        tab();
        printf("? parse_combined_affectation at %d\n", parser_index);
    }
#endif
    Node *n = parse_affectation(list);
    parser_level--;
    return n;
}

Node *parse_affectation(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    if (DEBUG_MODE == DEBUG_MODE_FULL)
    {
        tab();
        printf("? parse_affectation at %d\n", parser_index);
    }
#endif
    Node *n = parse_interval(list);
    parser_level--;
    return n;
}

Node *parse_interval(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    if (DEBUG_MODE == DEBUG_MODE_FULL)
    {
        tab();
        printf("? parse_interval at %d\n", parser_index);
    }
#endif
    Node *n = parse_logical_or(list);
    parser_level--;
    return n;
}

Node *parse_logical_or(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    if (DEBUG_MODE == DEBUG_MODE_FULL)
    {
        tab();
        printf("? parse_logical_or at %d\n", parser_index);
    }
#endif
    Node *node = parse_logical_and(list);
    while (check_token_value(list, parser_index, TOKEN_OPERATOR, "or"))
    {
#ifdef DEBUG
        tab();
        printf("> operator or found at %d!\n", parser_index);
#endif
        Node *left = node;
        Token t = token_list_get(list, parser_index);
        parser_index += 1;
        Node *right = parse_logical_and(list);
        node = (Node *)memory_get(sizeof(Node));
        node->token = t;
        node->left = left;
        node->right = right;
        node->type = NODE_BINARY_OPERATOR;
    }
    parser_level--;
    return node;
}

Node *parse_logical_and(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    if (DEBUG_MODE == DEBUG_MODE_FULL)
    {
        tab();
        printf("? parse_logical_and at %d\n", parser_index);
    }
#endif
    Node *node = parse_equality(list);
    while (check_token_value(list, parser_index, TOKEN_OPERATOR, "and"))
    {
#ifdef DEBUG
        tab();
        printf("> operator and found at %d!\n", parser_index);
#endif
        Node *left = node;
        Token t = token_list_get(list, parser_index);
        parser_index += 1;
        Node *right = parse_equality(list);
        node = (Node *)memory_get(sizeof(Node));
        node->token = t;
        node->left = left;
        node->right = right;
        node->type = NODE_BINARY_OPERATOR;
    }
    parser_level--;
    return node;
}

Node *parse_equality(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    if (DEBUG_MODE == DEBUG_MODE_FULL)
    {
        tab();
        printf("? parse_equality at %d\n", parser_index);
    }
#endif
    Node *node = parse_comparison(list);
    while (check_token_value(list, parser_index, TOKEN_OPERATOR, "=="))
    {
#ifdef DEBUG
        tab();
        printf("> operator == found at %d!\n", parser_index);
#endif
        Node *left = node;
        Token t = token_list_get(list, parser_index);
        parser_index += 1;
        Node *right = parse_comparison(list);
        node = (Node *)memory_get(sizeof(Node));
        node->token = t;
        node->left = left;
        node->right = right;
        node->type = NODE_BINARY_OPERATOR;
    }
    parser_level--;
    return node;
}

Node *parse_comparison(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    if (DEBUG_MODE == DEBUG_MODE_FULL)
    {
        tab();
        printf("? parse_comparison at %d\n", parser_index);
    }
#endif
    Node *n = parse_binary_or_xor(list);
    parser_level--;
    return n;
}

Node *parse_binary_or_xor(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    if (DEBUG_MODE == DEBUG_MODE_FULL)
    {
        tab();
        printf("? parse_binary_or_xor at %d\n", parser_index);
    }
#endif
    Node *n = parse_binary_and(list);
    parser_level--;
    return n;
}

Node *parse_binary_and(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    if (DEBUG_MODE == DEBUG_MODE_FULL)
    {
        tab();
        printf("? parse_binary_and at %d\n", parser_index);
    }
#endif
    Node *n = parse_shift(list);
    parser_level--;
    return n;
}

Node *parse_shift(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    if (DEBUG_MODE == DEBUG_MODE_FULL)
    {
        tab();
        printf("? parse_shift at %d\n", parser_index);
    }
#endif
    Node *n = parse_addition_soustraction(list);
    parser_level--;
    return n;
}

Node *parse_addition_soustraction(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    if (DEBUG_MODE == DEBUG_MODE_FULL)
    {
        tab();
        printf("? parse_addition_soustraction at %d\n", parser_index);
    }
#endif
    Node *node = parse_multiplication_division_modulo(list);
    while (check_token_value(list, parser_index, TOKEN_OPERATOR, "+") || check_token_value(list, parser_index, TOKEN_OPERATOR, "-"))
    {
#ifdef DEBUG
        tab();
        printf("> operator + found at %d!\n", parser_index);
#endif
        Node *left = node;
        Token t = token_list_get(list, parser_index);
        parser_index += 1;
        Node *right = parse_multiplication_division_modulo(list);
        node = (Node *)memory_get(sizeof(Node));
        node->token = t;
        node->left = left;
        node->right = right;
        node->type = NODE_BINARY_OPERATOR;
    }
    parser_level--;
    return node;
}

Node *parse_multiplication_division_modulo(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    if (DEBUG_MODE == DEBUG_MODE_FULL)
    {
        tab();
        printf("? parse_multiplication_division_modulo at %d\n", parser_index);
    }
#endif
    Node *node = parse_unary_minus(list);
    while (check_token_value(list, parser_index, TOKEN_OPERATOR, "*") || check_token_value(list, parser_index, TOKEN_OPERATOR, "/") || check_token_value(list, parser_index, TOKEN_OPERATOR, "%") || check_token_value(list, parser_index, TOKEN_OPERATOR, "//"))
    {
#ifdef DEBUG
        tab();
        printf("> operator *, /, %% found at %d!\n", parser_index);
#endif
        Node *left = node;
        Token t = token_list_get(list, parser_index);
        parser_index += 1;
        Node *right = parse_multiplication_division_modulo(list);
        node = (Node *)memory_get(sizeof(Node));
        node->token = t;
        node->left = left;
        node->right = right;
        node->type = NODE_BINARY_OPERATOR;
    }
    parser_level--;
    return node;
}

Node *parse_unary_minus(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    if (DEBUG_MODE == DEBUG_MODE_FULL)
    {
        tab();
        printf("? parse_unary_minus at %d\n", parser_index);
    }
#endif
    Node *n = parse_pow(list);
    parser_level--;
    return n;
}

Node *parse_pow(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    if (DEBUG_MODE == DEBUG_MODE_FULL)
    {
        tab();
        printf("? parse_pow at %d\n", parser_index);
    }
#endif
    Node *n = parse_unary_complement(list);
    parser_level--;
    return n;
}

Node *parse_unary_complement(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    if (DEBUG_MODE == DEBUG_MODE_FULL)
    {
        tab();
        printf("? parse_unary_complement at %d\n", parser_index);
    }
#endif
    Node *n = parse_call(list);
    parser_level--;
    return n;
}

Node *parse_call(TokenList *list)
{
    // left = function id | token = "(" | right = null | extra = null
    parser_level++;
#ifdef DEBUG
    if (DEBUG_MODE == DEBUG_MODE_FULL)
    {
        tab();
        printf("? parse_call at %d\n", parser_index);
    }
#endif
    Node *node = parse_litteral(list);
    if (check_token_value(list, parser_index, TOKEN_SEPARATOR, "("))
    {
#ifdef DEBUG
        tab();
        printf("> operator function call( %% found at %d!\n", parser_index);
#endif
        Node *left = node;
        Token t = token_list_get(list, parser_index);
        parser_index += 1; // opening (
        if (!check_token_value(list, parser_index, TOKEN_SEPARATOR, ")"))
        {
            general_error("arguments not supported yet.");
        }
        parser_index += 1; // closing )
        Node *right = NULL;
        node = (Node *)memory_get(sizeof(Node));
        node->token = t;
        node->left = left;
        node->right = right;
        node->type = NODE_FUNCTION_CALL;
    }
    parser_level--;
    return node;
}

Node *parse_litteral(TokenList *list)
{
    Node *node = NULL;
    parser_level++;
#ifdef DEBUG
    if (DEBUG_MODE == DEBUG_MODE_FULL)
    {
        tab();
        printf("? parse_litteral at %d\n", parser_index);
    }
#endif
    if (check_token_type(list, parser_index, TOKEN_DECIMAL))
    {
#ifdef DEBUG
        tab();
        printf("> litteral integer found at %d\n", parser_index);
#endif
        node = (Node *)memory_get(sizeof(Node));
        node->token = token_list_get(list, parser_index);
        // printf("parse_litteral check @text = %p, @count = %d, @start = %d\n", node->token.text, node->token.count, node->token.start);
        node->left = NULL;
        node->right = NULL;
        node->type = NODE_INTEGER;
        parser_index += 1;
    }
    else if (check_token_type(list, parser_index, TOKEN_FLOAT))
    {
#ifdef DEBUG
        tab();
        printf("> litteral flaot found at %d\n", parser_index);
#endif
        node = (Node *)memory_get(sizeof(Node));
        node->token = token_list_get(list, parser_index);
        node->left = NULL;
        node->right = NULL;
        node->type = NODE_FLOAT;
        parser_index += 1;
    }
    else if (check_token_type(list, parser_index, TOKEN_BOOLEAN))
    {
#ifdef DEBUG
        tab();
        printf("> litteral boolean found at %d\n", parser_index);
#endif
        node = (Node *)memory_get(sizeof(Node));
        node->token = token_list_get(list, parser_index);
        node->left = NULL;
        node->right = NULL;
        node->type = NODE_BOOLEAN;
        parser_index += 1;
    }
    else if (check_token_type(list, parser_index, TOKEN_STRING))
    {
#ifdef DEBUG
        tab();
        printf("> litteral string found at %d\n", parser_index);
#endif
        node = (Node *)memory_get(sizeof(Node));
        node->token = token_list_get(list, parser_index);
        node->left = NULL;
        node->right = NULL;
        node->type = NODE_STRING;
        parser_index += 1;
    }
    else if (check_token_type(list, parser_index, TOKEN_IDENTIFIER))
    {
#ifdef DEBUG
        tab();
        printf("> litteral identifier found at %d\n", parser_index);
#endif
        node = (Node *)memory_get(sizeof(Node));
        node->token = token_list_get(list, parser_index);
        node->left = NULL;
        node->right = NULL;
        node->type = NODE_IDENTIFIER;
        parser_index += 1;
    }
    else
    {
        general_error("Not a literal at %d", parser_index);
    }
    parser_level--;
    return node;
}

void spaces(uint32_t level)
{
    for (uint32_t i = 0; i < level; i++)
    {
        printf("    ");
    }
}

void node_print(Node *node)
{
    node_print_level(node, 0);
}

void node_print_level(Node *node, uint32_t level)
{
    // printf("DEBUG node_print_level (level=%d)\n", level);
    spaces(level);
    if (node->type == NODE_BINARY_OPERATOR)
    {
        printf("BINARY OPERATOR %.*s\n", node->token.count, node->token.text + node->token.start);
        node_print_level(node->left, level + 1);
        node_print_level(node->right, level + 1);
    }
    else if (node->type == NODE_INTEGER)
    {
        // printf("@text = %p, @count = %d, @start = %d\n", node->token.text, node->token.count, node->token.start);
        printf("INTEGER %.*s\n", node->token.count, node->token.text + node->token.start);
    }
    else if (node->type == NODE_FLOAT)
    {
        printf("FLOAT %.*s\n", node->token.count, node->token.text + node->token.start);
    }
    else if (node->type == NODE_BOOLEAN)
    {
        printf("BOOLEAN %.*s\n", node->token.count, node->token.text + node->token.start);
    }
    else if (node->type == NODE_STRING)
    {
        printf("STRING %.*s\n", node->token.count, node->token.text + node->token.start);
    }
    else if (node->type == NODE_FUNCTION_CALL)
    {
        printf("FUNCTION CALL print\n");
    }
    else if (node->type == NODE_BLOCK)
    {
        printf("BLOCK\n");
        spaces(level + 1);
        printf("LEFT\n");
        node_print_level(node->left, level + 2);
        spaces(level + 1);
        printf("RIGHT\n"); // Never null !
        node_print_level(node->right, level + 2);
    }
    else if (node->type == NODE_IF)
    {
        printf("IF\n");
        spaces(level + 1);
        printf("CONDITION\n");
        node_print_level(node->extra, level + 2);
        spaces(level + 1);
        printf("ACTION\n");
        node_print_level(node->left, level + 2);
    }
    else
    {
        general_error("Parser: Unknown node type: %d", node->type);
    }
}

void ast_print(Tree *tree)
{
    node_print_level(tree->root, 0);
}
