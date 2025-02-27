#include "parser.h"

unsigned int parser_index = 0;

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
    //printf("%d vs expected: %d\n", t.type, expected_type);
    //printf("expected: %s\n", expected_value);
    //token_print(t);
    return t.type == expected_type && token_cmp(t, expected_value);
}

Tree *parse(TokenList *list)
{
    parser_index = 0;
#ifdef DEBUG
    printf("parse at %d\n", parser_index);
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
#ifdef DEBUG
    printf("    parse_expression at %d\n", parser_index);
#endif
    return parse_addition(list);
}

Node *parse_addition(TokenList *list)
{
#ifdef DEBUG
    printf("        parse_addition at %d\n", parser_index);
#endif
    Node *node = parse_multiplication(list);
    if (check_value(list, parser_index, TOKEN_OPERATOR, "+"))
    {
        #ifdef DEBUG
        printf("        operator + found at %d!\n", parser_index);
        #endif
        Node *left = node;
        Token t = token_list_get(list, parser_index);
        parser_index += 1;
        Node *right = parse_multiplication(list);
        node = (Node *)memory_get(sizeof(Node));
        node->token = t;
        node->left = left;
        node->right = right;
        node->type = NODE_BINARY_OPERATOR;
    }
    return node;
}

Node *parse_multiplication(TokenList *list)
{
#ifdef DEBUG
    printf("            parse_multiplication at %d\n", parser_index);
#endif
    Node *node = parse_litteral(list);
    if (check_value(list, parser_index, TOKEN_OPERATOR, "*"))
    {
        #ifdef DEBUG
        printf("            operator * found at %d!\n", parser_index);
        #endif
        Node *left = node;
        Token t = token_list_get(list, parser_index);
        parser_index += 1;
        Node *right = parse_multiplication(list);
        node = (Node *)memory_get(sizeof(Node));
        node->token = t;
        node->left = left;
        node->right = right;
        node->type = NODE_BINARY_OPERATOR;
    }
    return node;
}

Node *parse_litteral(TokenList *list)
{
#ifdef DEBUG
    printf("                parse_litteral at %d\n", parser_index);
#endif
    if (check_type(list, parser_index, TOKEN_DECIMAL))
    {
        #ifdef DEBUG
        printf("                litteral found at %d!\n", parser_index);
        #endif
        Node *node = (Node *)memory_get(sizeof(Node));
        node->token = token_list_get(list, parser_index);
        //printf("parse_litteral check @text = %p, @count = %d, @start = %d\n", node->token.text, node->token.count, node->token.start);
        node->left = NULL;
        node->right = NULL;
        node->type = NODE_INTEGER;
        parser_index += 1;
        return node;
    }
    else
    {
        printf("NOT A LITTERAL AT %d\n", parser_index);
        general_error("NOT A LITTERAL ERROR");
    }
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
        //printf("@text = %p, @count = %d, @start = %d\n", node->token.text, node->token.count, node->token.start);
        printf("INTEGER %.*s\n", node->token.count, node->token.text + node->token.start);
    }
    else
    {
        printf("Node type : %d\n", node->type);
        general_error("ERROR: UNKNOWN NODE TYPE");
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
