#include "parser.h"

#define SPACES 4

unsigned int parser_index = 0;
unsigned int parser_level = 0;

bool check_type(TokenList *list, uint32_t index, TokenType expected)
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

bool check_value(TokenList *list, uint32_t index, TokenType expected_type, const char *expected_value)
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
    printf("? parse at %d\n", parser_index);
#endif
    Tree *t = (Tree *)memory_get(sizeof(Tree));
    t->root = parse_expression(list);
#ifdef DEBUG
    printf("end of parsing at %d / %d\n", parser_index, token_list_size(list));
#endif
    return t;
}

Node *parse_expression(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    tab();
    printf("? parse_expression at %d\n", parser_index);
#endif
    Node *n = parse_combined_affectation_binary(list);
    parser_level--;
    return n;
}

Node *parse_combined_affectation_binary(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    tab();
    printf("? parse_combined_affectation_binary at %d\n", parser_index);
#endif
    Node *n = parse_combined_affectation_shift(list);
    parser_level--;
    return n;
}

Node *parse_combined_affectation_shift(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    tab();
    printf("? parse_combined_affectation_shift at %d\n", parser_index);
#endif
    Node *n = parse_combined_affectation(list);
    parser_level--;
    return n;
}

Node *parse_combined_affectation(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    tab();
    printf("? parse_combined_affectation at %d\n", parser_index);
#endif
    Node *n = parse_affectation(list);
    parser_level--;
    return n;
}

Node *parse_affectation(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    tab();
    printf("? parse_affectation at %d\n", parser_index);
#endif
    Node *n = parse_interval(list);
    parser_level--;
    return n;
}

Node *parse_interval(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    tab();
    printf("? parse_interval at %d\n", parser_index);
#endif
    Node *n = parse_logical_or(list);
    parser_level--;
    return n;
}

Node *parse_logical_or(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    tab();
    printf("? parse_logical_or at %d\n", parser_index);
#endif
    Node *n = parse_logical_and(list);
    parser_level--;
    return n;
}

Node *parse_logical_and(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    tab();
    printf("? parse_logical_and at %d\n", parser_index);
#endif
    Node *n = parse_equality(list);
    parser_level--;
    return n;
}

Node *parse_equality(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    tab();
    printf("? parse_equality at %d\n", parser_index);
#endif
    Node *n = parse_comparison(list);
    parser_level--;
    return n;
}

Node *parse_comparison(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    tab();
    printf("? parse_comparison at %d\n", parser_index);
#endif
    Node *n = parse_binary_or_xor(list);
    parser_level--;
    return n;
}

Node *parse_binary_or_xor(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    tab();
    printf("? parse_binary_or_xor at %d\n", parser_index);
#endif
    Node *n = parse_binary_and(list);
    parser_level--;
    return n;
}

Node *parse_binary_and(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    tab();
    printf("? parse_binary_and at %d\n", parser_index);
#endif
    Node *n = parse_shift(list);
    parser_level--;
    return n;
}

Node *parse_shift(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    tab();
    printf("? parse_shift at %d\n", parser_index);
#endif
    Node *n = parse_addition_soustraction(list);
    parser_level--;
    return n;
}

Node *parse_addition_soustraction(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    tab();
    printf("? parse_addition at %d\n", parser_index);
#endif
    Node *node = parse_multiplication_division_modulo(list);
    while (check_value(list, parser_index, TOKEN_OPERATOR, "+")
            || check_value(list, parser_index, TOKEN_OPERATOR, "-"))
    {
#ifdef DEBUG
        tab();
        printf("! operator + found at %d!\n", parser_index);
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
    tab();
    printf("? parse_multiplication at %d\n", parser_index);
#endif
    Node *node = parse_unary_minus(list);
    while (check_value(list, parser_index, TOKEN_OPERATOR, "*")
        || check_value(list, parser_index, TOKEN_OPERATOR, "/")
        || check_value(list, parser_index, TOKEN_OPERATOR, "%"))
    {
#ifdef DEBUG
        tab();
        printf("! operator *, /, %% found at %d!\n", parser_index);
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
    tab();
    printf("? parse_unary_minus at %d\n", parser_index);
#endif
    Node *n = parse_pow(list);
    parser_level--;
    return n;
}

Node *parse_pow(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    tab();
    printf("? parse_pow at %d\n", parser_index);
#endif
    Node *n = parse_unary_complement(list);
    parser_level--;
    return n;
}

Node *parse_unary_complement(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    tab();
    printf("? parse_unary_complement at %d\n", parser_index);
#endif
    Node *n = parse_litteral(list);
    parser_level--;
    return n;
}

Node *parse_litteral(TokenList *list)
{
    parser_level++;
#ifdef DEBUG
    tab();
    printf("? parse_litteral at %d\n", parser_index);
#endif
    if (check_type(list, parser_index, TOKEN_DECIMAL))
    {
#ifdef DEBUG
        tab();
        printf("! litteral found at %d\n", parser_index);
#endif
        Node *node = (Node *)memory_get(sizeof(Node));
        node->token = token_list_get(list, parser_index);
        // printf("parse_litteral check @text = %p, @count = %d, @start = %d\n", node->token.text, node->token.count, node->token.start);
        node->left = NULL;
        node->right = NULL;
        node->type = NODE_INTEGER;
        parser_index += 1;
        parser_level--;
        return node;
    }
    else
    {
        general_error("Not a literal at %d", parser_index);
    }
    parser_level--;
    return NULL;
}

void node_print(Node *node, uint32_t level)
{
    for (uint32_t i = 0; i < level; i++)
    {
        printf("    ");
    }
    if (node->type == NODE_BINARY_OPERATOR)
    {
        printf("BINARY OPERATOR %.*s\n", node->token.count, node->token.text + node->token.start);
    }
    else if (node->type == NODE_INTEGER)
    {
        // printf("@text = %p, @count = %d, @start = %d\n", node->token.text, node->token.count, node->token.start);
        printf("INTEGER %.*s\n", node->token.count, node->token.text + node->token.start);
    }
    else
    {
        general_error("Unknown node type: %d", node->type);
    }
    if (node->left != NULL)
    {
        node_print(node->left, level + 1);
    }
    if (node->right != NULL)
    {
        node_print(node->right, level + 1);
    }
}

void ast_print(Tree *tree)
{
    node_print(tree->root, 0);
}
