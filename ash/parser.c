#include "parser.h"

#define SPACES 4

const char *NODE_TYPE_REPR_STRING[] = {
    "NODE_INTEGER",
    "NODE_FLOAT",
    "NODE_BOOLEAN",
    "NODE_STRING",
    "NODE_IDENTIFIER",
    "NODE_BINARY_OPERATOR",
    "NODE_IF",
    "NODE_WHILE",
    "NODE_FUNCTION_CALL",
    "NODE_BLOCK"};

uint8_t DEBUG_MODE = DEBUG_MODE_FULL; // DEBUG_MODE_CLEVER;
Dict *root_scope = NULL;

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

bool parser_debug = false;

bool parser_set_debug(bool debug)
{
    parser_debug = debug;
    return parser_debug;
}

bool parser_get_debug()
{
    return parser_debug;
}

AST *parse(TokenList *list)
{
    if (root_scope == NULL)
    {
        root_scope = dict_init(VALUE_CSTRING, VALUE_TYPE);
        // dict_free(root_scope);
    }
    parser_index = 0;
    parser_level = 0;
    if (parser_debug)
    {
        printf("> parse at %d\n", parser_index);
    }
    AST *t = (AST *)memory_get(sizeof(AST));
    t->root = parse_block(list);
    if (parser_index != token_list_size(list))
    {
        general_message(FATAL, "Tokens not parsed at the end of the token list (>= %d).", parser_index);
    }
    if (parser_debug)
    {
        printf("end of parsing at %d / %d\n", parser_index, token_list_size(list));
    }
    return t;
}

Node *parse_block(TokenList *list)
{
    parser_level++;
    if (parser_debug)
    {
        tab();
        printf("> parse_block at %d\n", parser_index);
    }
    Node *first = NULL;
    Node *node = NULL;
    Node *next = NULL;
    do
    {
        while (check_token_value(list, parser_index, TOKEN_SEPARATOR, ";") || check_token_value(list, parser_index, TOKEN_NEWLINE, "\n"))
        {
            parser_index += 1;
        }
        if (check_token_value(list, parser_index, TOKEN_KEYWORD, "if"))
        {
            next = parse_if(list);
        }
        else if (check_token_value(list, parser_index, TOKEN_KEYWORD, "while"))
        {
            next = parse_while(list);
        }
        else if (check_token_value(list, parser_index, TOKEN_KEYWORD, "loop") || check_token_value(list, parser_index, TOKEN_KEYWORD, "end"))
        {
            break;
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
                block->value_type = VALUE_NIL;
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
                block->value_type = VALUE_NIL;
                block->token = node->token;
                block->left = node;
                block->right = next;
                node = block;
                first = node;
            }
        }
        next = NULL;
    } while ((check_token_value(list, parser_index, TOKEN_SEPARATOR, ";") || check_token_value(list, parser_index, TOKEN_NEWLINE, "\n")) && parser_index < token_list_size(list));
    return first;
}

Node *parse_if(TokenList *list)
{
    parser_level++;
    if (parser_debug)
    {
        tab();
        printf("> parse_if at %d\n", parser_index);
    }
    Node *node = node = (Node *)memory_get(sizeof(Node));
    node->type = NODE_IF;
    node->value_type = VALUE_NIL;
    Token t = token_list_get(list, parser_index);
    node->token = t;   // keyword if
    parser_index += 1; // pass keyword if
    node->extra = parse_expression(list);
    if (!check_token_value(list, parser_index, TOKEN_KEYWORD, "then"))
    {
        general_message(FATAL, "if not followed by then.");
    }
    if (parser_debug)
    {
        tab();
        printf("> parse THEN\n");
    }
    parser_index += 1; // pass keyword then
    node->left = parse_block(list);
    node->right = NULL;
    Node *first_if = node;
    while (check_token_value(list, parser_index, TOKEN_KEYWORD, "elsif"))
    {
        if (parser_debug)
        {
            tab();
            printf("> parse ELSIF\n");
        }
        Node *elsif = (Node *)memory_get(sizeof(Node));
        elsif->type = NODE_IF;
        elsif->value_type = VALUE_NIL;
        parser_index += 1; // pass keyword elsif
        elsif->extra = parse_expression(list);
        if (!check_token_value(list, parser_index, TOKEN_KEYWORD, "then"))
        {
            general_message(FATAL, "elsif not followed by then.");
        }
        parser_index += 1; // pass keyword then
        elsif->left = parse_block(list);
        elsif->right = NULL;
        node->right = elsif;
        node = elsif;
    }
    if (check_token_value(list, parser_index, TOKEN_KEYWORD, "else"))
    {
        if (parser_debug)
        {
            tab();
            printf("> parse ELSE\n");
        }
        parser_index += 1; // pass keyword else
        node->right = parse_block(list);
    }
    if (!check_token_value(list, parser_index, TOKEN_KEYWORD, "end"))
    {
        general_message(FATAL, "if not terminated by end.");
    }
    if (parser_debug)
    {
        tab();
        printf("> parse END\n");
    }
    parser_index += 1; // pass keyword end
    parser_level--;
    return first_if;
}

Node *parse_while(TokenList *list)
{
    parser_level++;
    if (parser_debug)
    {
        tab();
        printf("> parse_while at %d\n", parser_index);
    }
    Node *node = node = (Node *)memory_get(sizeof(Node));
    node->type = NODE_WHILE;
    node->value_type = VALUE_NIL;
    Token t = token_list_get(list, parser_index);
    node->token = t;   // keyword while
    parser_index += 1; // pass keyword while
    node->extra = parse_expression(list);
    if (!check_token_value(list, parser_index, TOKEN_KEYWORD, "do"))
    {
        general_message(FATAL, "while not followed by do.");
    }
    if (parser_debug)
    {
        tab();
        printf("> parse DO\n");
    }
    parser_index += 1; // pass keyword do
    node->left = parse_block(list);
    node->right = NULL;
    if (!check_token_value(list, parser_index, TOKEN_KEYWORD, "loop"))
    {
        general_message(FATAL, "while not terminated by loop.");
    }
    if (parser_debug)
    {
        tab();
        printf("> parse LOOP\n");
    }
    parser_index += 1; // pass keyword loop
    parser_level--;
    return node;
}

Node *parse_expression(TokenList *list)
{
    parser_level++;
    if (parser_debug)
    {
        if (DEBUG_MODE == DEBUG_MODE_FULL)
        {
            tab();
            printf("> parse_expression at %d\n", parser_index);
        }
    }
    Node *n = parse_combined_affectation_binary(list);
    parser_level--;
    return n;
}

Node *parse_combined_affectation_binary(TokenList *list)
{
    parser_level++;
    if (parser_debug)
    {
        if (DEBUG_MODE == DEBUG_MODE_FULL)
        {
            tab();
            printf("? parse_combined_affectation_binary at %d\n", parser_index);
        }
    }
    Node *n = parse_combined_affectation_shift(list);
    parser_level--;
    return n;
}

Node *parse_combined_affectation_shift(TokenList *list)
{
    parser_level++;
    if (parser_debug)
    {
        if (DEBUG_MODE == DEBUG_MODE_FULL)
        {
            tab();
            printf("? parse_combined_affectation_shift at %d\n", parser_index);
        }
    }
    Node *n = parse_combined_affectation(list);
    parser_level--;
    return n;
}

Node *parse_combined_affectation(TokenList *list)
{
    parser_level++;
    if (parser_debug)
    {
        if (DEBUG_MODE == DEBUG_MODE_FULL)
        {
            tab();
            printf("? parse_combined_affectation at %d\n", parser_index);
        }
    }
    Node *n = parse_affectation(list);
    parser_level--;
    return n;
}

Node *parse_affectation(TokenList *list)
{
    parser_level++;
    if (parser_debug)
    {
        if (DEBUG_MODE == DEBUG_MODE_FULL)
        {
            tab();
            printf("? parse_affectation at %d\n", parser_index);
        }
    }
    Node *node = NULL;
    // :TODO check if parser_index + 1 in list
    if (check_token_type(list, parser_index, TOKEN_IDENTIFIER) && check_token_value(list, parser_index + 1, TOKEN_OPERATOR, "="))
    {
        if (parser_debug)
        {
            tab();
            printf("> operator = found at %d!\n", parser_index);
        }
        // Affectation
        unsigned int where_it_begins = parser_index;
        parser_index += 1;
        Token t = token_list_get(list, parser_index); // to get the '='
        parser_index += 1;                            // to remove the '='
        Node *right = parse_affectation(list);
        unsigned int where_it_ends = parser_index;
        parser_index = where_it_begins;
        printf("Right value type = %s\n", VALUE_TYPE_STRING[right->value_type]);
        Node *left = parse_identifier_left_aff(list, right->value_type);
        parser_index = where_it_ends;
        node = (Node *)memory_get(sizeof(Node));
        node->token = t;
        node->left = left;
        node->right = right;
        node->type = NODE_BINARY_OPERATOR;
        node->value_type = node->right->value_type;
    }
    else
    {
        // On passe après
        node = parse_interval(list);
    }
    parser_level--;
    return node;
    // Recyclé
    // Node *node = parse_identifier_left_aff(list);
    // while (check_token_value(list, parser_index, TOKEN_OPERATOR, "="))
    // {
    // }
}

Node *parse_interval(TokenList *list)
{
    parser_level++;
    if (parser_debug)
    {
        if (DEBUG_MODE == DEBUG_MODE_FULL)
        {
            tab();
            printf("? parse_interval at %d\n", parser_index);
        }
    }
    Node *n = parse_logical_or(list);
    parser_level--;
    return n;
}

Node *parse_logical_or(TokenList *list)
{
    parser_level++;
    if (parser_debug)
    {
        if (DEBUG_MODE == DEBUG_MODE_FULL)
        {
            tab();
            printf("? parse_logical_or at %d\n", parser_index);
        }
    }
    Node *node = parse_logical_and(list);
    while (check_token_value(list, parser_index, TOKEN_OPERATOR, "or"))
    {
        if (parser_debug)
        {
            tab();
            printf("> operator or found at %d!\n", parser_index);
        }
        Node *left = node;
        Token t = token_list_get(list, parser_index);
        parser_index += 1;
        Node *right = parse_logical_and(list);
        node = (Node *)memory_get(sizeof(Node));
        node->token = t;
        node->left = left;
        node->right = right;
        node->type = NODE_BINARY_OPERATOR;
        node->value_type = VALUE_BOOLEAN;
    }
    parser_level--;
    return node;
}

Node *parse_logical_and(TokenList *list)
{
    parser_level++;
    if (parser_debug)
    {
        if (DEBUG_MODE == DEBUG_MODE_FULL)
        {
            tab();
            printf("? parse_logical_and at %d\n", parser_index);
        }
    }
    Node *node = parse_equality(list);
    while (check_token_value(list, parser_index, TOKEN_OPERATOR, "and"))
    {
        if (parser_debug)
        {
            tab();
            printf("> operator and found at %d!\n", parser_index);
        }
        Node *left = node;
        Token t = token_list_get(list, parser_index);
        parser_index += 1;
        Node *right = parse_equality(list);
        node = (Node *)memory_get(sizeof(Node));
        node->token = t;
        node->left = left;
        node->right = right;
        node->type = NODE_BINARY_OPERATOR;
        node->value_type = VALUE_BOOLEAN;
    }
    parser_level--;
    return node;
}

Node *parse_equality(TokenList *list)
{
    parser_level++;
    if (parser_debug)
    {
        if (DEBUG_MODE == DEBUG_MODE_FULL)
        {
            tab();
            printf("? parse_equality at %d\n", parser_index);
        }
    }
    Node *node = parse_comparison(list);
    while (check_token_value(list, parser_index, TOKEN_OPERATOR, "==") || check_token_value(list, parser_index, TOKEN_OPERATOR, "!="))
    {
        if (parser_debug)
        {
            tab();
            printf("> operator == or != found at %d!\n", parser_index);
        }
        Node *left = node;
        Token t = token_list_get(list, parser_index);
        parser_index += 1;
        Node *right = parse_comparison(list);
        node = (Node *)memory_get(sizeof(Node));
        node->token = t;
        node->left = left;
        node->right = right;
        node->type = NODE_BINARY_OPERATOR;
        node->value_type = VALUE_BOOLEAN;
    }
    parser_level--;
    return node;
}

Node *parse_comparison(TokenList *list)
{
    parser_level++;
    if (parser_debug)
    {
        if (DEBUG_MODE == DEBUG_MODE_FULL)
        {
            tab();
            printf("? parse_comparison at %d\n", parser_index);
        }
    }
    Node *node = parse_binary_or_xor(list);
    if (check_token_value(list, parser_index, TOKEN_OPERATOR, "<") || check_token_value(list, parser_index, TOKEN_OPERATOR, ">"))
    {
        if (parser_debug)
        {
            tab();
            printf("> operator < or > found at %d!\n", parser_index);
        }
        Node *left = node;
        Token t = token_list_get(list, parser_index);
        parser_index += 1;
        Node *right = parse_comparison(list);
        node = (Node *)memory_get(sizeof(Node));
        node->token = t;
        node->left = left;
        node->right = right;
        node->type = NODE_BINARY_OPERATOR;
        node->value_type = VALUE_BOOLEAN;
    }
    parser_level--;
    return node;
}

Node *parse_binary_or_xor(TokenList *list)
{
    parser_level++;
    if (parser_debug)
    {
        if (DEBUG_MODE == DEBUG_MODE_FULL)
        {
            tab();
            printf("? parse_binary_or_xor at %d\n", parser_index);
        }
    }
    Node *n = parse_binary_and(list);
    parser_level--;
    return n;
}

Node *parse_binary_and(TokenList *list)
{
    parser_level++;
    if (parser_debug)
    {
        if (DEBUG_MODE == DEBUG_MODE_FULL)
        {
            tab();
            printf("? parse_binary_and at %d\n", parser_index);
        }
    }
    Node *n = parse_shift(list);
    parser_level--;
    return n;
}

Node *parse_shift(TokenList *list)
{
    parser_level++;
    if (parser_debug)
    {
        if (DEBUG_MODE == DEBUG_MODE_FULL)
        {
            tab();
            printf("? parse_shift at %d\n", parser_index);
        }
    }
    Node *n = parse_addition_soustraction(list);
    parser_level--;
    return n;
}

Node *parse_addition_soustraction(TokenList *list)
{
    parser_level++;
    if (parser_debug)
    {
        if (DEBUG_MODE == DEBUG_MODE_FULL)
        {
            tab();
            printf("? parse_addition_soustraction at %d\n", parser_index);
        }
    }
    Node *node = parse_multiplication_division_modulo(list);
    while (check_token_value(list, parser_index, TOKEN_OPERATOR, "+") || check_token_value(list, parser_index, TOKEN_OPERATOR, "-"))
    {
        if (parser_debug)
        {
            tab();
            printf("> operator + found at %d!\n", parser_index);
        }
        Node *left = node;
        Token t = token_list_get(list, parser_index);
        parser_index += 1;
        Node *right = parse_multiplication_division_modulo(list);
        node = (Node *)memory_get(sizeof(Node));
        node->token = t;
        node->left = left;
        node->right = right;
        node->type = NODE_BINARY_OPERATOR;
        if (node->left->value_type == VALUE_INTEGER && node->right->value_type == VALUE_INTEGER)
        {
            node->value_type = VALUE_INTEGER;
        }
        else if ((node->left->value_type == VALUE_INTEGER && node->right->value_type == VALUE_FLOAT) ||
                 (node->left->value_type == VALUE_FLOAT && node->right->value_type == VALUE_INTEGER) ||
                 (node->left->value_type == VALUE_FLOAT && node->right->value_type == VALUE_FLOAT))
        {
            node->value_type = VALUE_FLOAT;
        }
        else if (node->left->value_type == VALUE_STRING && node->right->value_type == VALUE_STRING)
        {
            node->value_type = VALUE_STRING;
        }
        else
        {
            general_message(FATAL, "Parser : impossible to resolve type between left=%s and right=%s for addition soustraction.", VALUE_TYPE_STRING[node->left->value_type], VALUE_TYPE_STRING[node->right->value_type]);
        }
    }
    parser_level--;
    return node;
}

Node *parse_multiplication_division_modulo(TokenList *list)
{
    parser_level++;
    if (parser_debug)
    {
        if (DEBUG_MODE == DEBUG_MODE_FULL)
        {
            tab();
            printf("? parse_multiplication_division_modulo at %d\n", parser_index);
        }
    }
    Node *node = parse_unary_minus(list);
    while (check_token_value(list, parser_index, TOKEN_OPERATOR, "*") || check_token_value(list, parser_index, TOKEN_OPERATOR, "/") || check_token_value(list, parser_index, TOKEN_OPERATOR, "%") || check_token_value(list, parser_index, TOKEN_OPERATOR, "//"))
    {
        if (parser_debug)
        {
            tab();
            printf("> operator *, /, %% found at %d!\n", parser_index);
        }
        Node *left = node;
        Token t = token_list_get(list, parser_index);
        parser_index += 1;
        Node *right = parse_multiplication_division_modulo(list);
        node = (Node *)memory_get(sizeof(Node));
        node->token = t;
        node->left = left;
        node->right = right;
        node->type = NODE_BINARY_OPERATOR;
        if (token_cmp(node->token, "*"))
        {
            if (node->left->value_type == VALUE_INTEGER && node->right->value_type == VALUE_INTEGER)
            {
                node->value_type = VALUE_INTEGER;
            }
            else if ((node->left->value_type == VALUE_INTEGER && node->right->value_type == VALUE_FLOAT) ||
                     (node->left->value_type == VALUE_FLOAT && node->right->value_type == VALUE_INTEGER) ||
                     (node->left->value_type == VALUE_FLOAT && node->right->value_type == VALUE_FLOAT))
            {
                node->value_type = VALUE_FLOAT;
            }
            else if ((node->left->value_type == VALUE_STRING && node->right->value_type == VALUE_INTEGER) ||
                     (node->left->value_type == VALUE_INTEGER && node->right->value_type == VALUE_STRING))
            {
                node->value_type = VALUE_STRING;
            }
            else
            {
                general_message(FATAL, "Parser : impossible to resolve type between left=%s and right=%s for multiplication.", VALUE_TYPE_STRING[node->left->value_type], VALUE_TYPE_STRING[node->right->value_type]);
            }
        }
        else if (token_cmp(node->token, "/"))
        {
            // :TODO:
        }
        else if (token_cmp(node->token, "//"))
        {
            // :TODO:
        }
        else if (token_cmp(node->token, "%"))
        {
            // :TODO:
        }
    }
    parser_level--;
    return node;
}

Node *parse_unary_minus(TokenList *list)
{
    parser_level++;
    if (parser_debug)
    {
        if (DEBUG_MODE == DEBUG_MODE_FULL)
        {
            tab();
            printf("? parse_unary_minus at %d\n", parser_index);
        }
    }
    Node *n = parse_pow(list);
    parser_level--;
    return n;
}

Node *parse_pow(TokenList *list)
{
    parser_level++;
    if (parser_debug)
    {
        if (DEBUG_MODE == DEBUG_MODE_FULL)
        {
            tab();
            printf("? parse_pow at %d\n", parser_index);
        }
    }
    Node *n = parse_unary_complement(list);
    parser_level--;
    return n;
}

Node *parse_unary_complement(TokenList *list)
{
    parser_level++;
    if (parser_debug)
    {
        if (DEBUG_MODE == DEBUG_MODE_FULL)
        {
            tab();
            printf("? parse_unary_complement at %d\n", parser_index);
        }
    }
    Node *n = parse_call(list);
    parser_level--;
    return n;
}

Node *parse_call(TokenList *list)
{
    // left = function id | token = "(" | right = null | extra = null
    parser_level++;
    if (parser_debug)
    {
        if (DEBUG_MODE == DEBUG_MODE_FULL)
        {
            tab();
            printf("? parse_call at %d\n", parser_index);
        }
    }
    // :TODO: Warning : unhandled case if I do (expr)() !
    Node *node = parse_parenthesis_expr(list);
    if (check_token_value(list, parser_index, TOKEN_SEPARATOR, "("))
    {
        if (parser_debug)
        {
            tab();
            printf("> operator function call( %% found at %d!\n", parser_index);
        }
        Node *left = node;
        Token t = token_list_get(list, parser_index);
        parser_index += 1; // opening (
        if (!check_token_value(list, parser_index, TOKEN_SEPARATOR, ")"))
        {
            general_message(FATAL, "arguments not supported yet.");
        }
        parser_index += 1; // closing )
        Node *right = NULL;
        node = (Node *)memory_get(sizeof(Node));
        node->token = t;
        node->left = left;
        node->right = right;
        node->type = NODE_FUNCTION_CALL;
        // :TODO: List of function to resolte type
    }
    parser_level--;
    return node;
}

Node *parse_identifier_left_aff(TokenList *list, ValueType type_to_set)
{
    Node *node = NULL;
    parser_level++;
    if (!check_token_type(list, parser_index, TOKEN_IDENTIFIER))
    {
        general_message(FATAL, "This function is only for left part of affectation.\n");
    }
    node = (Node *)memory_get(sizeof(Node));
    node->token = token_list_get(list, parser_index);
    node->left = NULL;
    node->right = NULL;
    node->type = NODE_IDENTIFIER;
    node->value_type = type_to_set;
    parser_index += 1;
    // Registering or checking variable
    char *tval = token_value(node->token);
    Value var_name = cstring_init(tval);
    Value value_type_to_set = type_init(type_to_set);
    value_print_message("Searching for key %v in root_scope.\n", var_name);
    dict_print(root_scope);
    if (dict_key_exists(root_scope, var_name))
    {
        printf("Variable found in root_scope!\n");
        Value value_type_registered = dict_get(root_scope, var_name);
        if (!equality(value_type_to_set, value_type_registered) && (value_type_registered.as.integer != VALUE_FLOAT || value_type_to_set.as.integer != VALUE_INTEGER)) // type must be the same except if we try to put an int into a float
        {
            general_message(FATAL, "Impossible to put a value of type %s into the variable %s of type %s", VALUE_TYPE_STRING[value_type_to_set.as.integer], tval, VALUE_TYPE_STRING[value_type_registered.as.integer]);
        }
    }
    else
    {
        printf("Variable not found in root_scope! Adding:\n");
        dict_set(root_scope, var_name, value_type_to_set);
        dict_print(root_scope);
    }
    return node;
}

Node *parse_parenthesis_expr(TokenList *list)
{
    Node *node = NULL;
    parser_level++;
    if (parser_debug)
    {
        if (DEBUG_MODE == DEBUG_MODE_FULL)
        {
            tab();
            printf("? parse_parenthesis_expr at %d\n", parser_index);
        }
    }
    if (check_token_value(list, parser_index, TOKEN_SEPARATOR, "("))
    {
        if (parser_debug)
        {
            if (DEBUG_MODE == DEBUG_MODE_FULL)
            {
                tab();
                printf("! parse_parenthesis_expr at %d\n", parser_index);
            }
        }
        parser_index += 1; // skip '('
        node = parse_expression(list);
        if (!check_token_value(list, parser_index, TOKEN_SEPARATOR, ")"))
        {
            general_message(FATAL, "Unclosed parenthsis");
        }
        parser_index += 1; // skip ')'
    }
    else
    {
        node = parse_litteral(list);
    }
    parser_level--;
    return node;
}

Node *parse_litteral(TokenList *list)
{
    Node *node = NULL;
    parser_level++;
    if (parser_debug)
    {
        if (DEBUG_MODE == DEBUG_MODE_FULL)
        {
            tab();
            printf("? parse_litteral at %d\n", parser_index);
        }
    }
    if (check_token_type(list, parser_index, TOKEN_DECIMAL))
    {
        if (parser_debug)
        {
            tab();
            printf("> litteral integer found at %d\n", parser_index);
        }
        node = (Node *)memory_get(sizeof(Node));
        node->token = token_list_get(list, parser_index);
        // printf("parse_litteral check @text = %p, @count = %d, @start = %d\n", node->token.text, node->token.count, node->token.start);
        node->left = NULL;
        node->right = NULL;
        node->type = NODE_INTEGER;
        node->value_type = VALUE_INTEGER;
        parser_index += 1;
    }
    else if (check_token_type(list, parser_index, TOKEN_FLOAT))
    {
        if (parser_debug)
        {
            tab();
            printf("> litteral flaot found at %d\n", parser_index);
        }
        node = (Node *)memory_get(sizeof(Node));
        node->token = token_list_get(list, parser_index);
        node->left = NULL;
        node->right = NULL;
        node->type = NODE_FLOAT;
        node->value_type = VALUE_FLOAT;
        parser_index += 1;
    }
    else if (check_token_type(list, parser_index, TOKEN_BOOLEAN))
    {
        if (parser_debug)
        {
            tab();
            printf("> litteral boolean found at %d\n", parser_index);
        }
        node = (Node *)memory_get(sizeof(Node));
        node->token = token_list_get(list, parser_index);
        node->left = NULL;
        node->right = NULL;
        node->type = NODE_BOOLEAN;
        node->value_type = VALUE_BOOLEAN;
        parser_index += 1;
    }
    else if (check_token_type(list, parser_index, TOKEN_STRING))
    {
        if (parser_debug)
        {
            tab();
            printf("> litteral string found at %d\n", parser_index);
        }
        node = (Node *)memory_get(sizeof(Node));
        node->token = token_list_get(list, parser_index);
        node->left = NULL;
        node->right = NULL;
        node->type = NODE_STRING;
        node->value_type = VALUE_STRING;
        parser_index += 1;
    }
    else if (check_token_type(list, parser_index, TOKEN_IDENTIFIER))
    {
        if (parser_debug)
        {
            tab();
            printf("> litteral identifier found at %d\n", parser_index);
        }
        node = (Node *)memory_get(sizeof(Node));
        node->token = token_list_get(list, parser_index);
        node->left = NULL;
        node->right = NULL;
        node->type = NODE_IDENTIFIER;
        // List of var with their type
        char *tval = token_value(node->token);
        Value key = cstring_init(tval);
        if (dict_key_exists(root_scope, key))
        {
            dict_print(root_scope);
            Value value = dict_get(root_scope, key);
            node->value_type = value.as.integer;
        }
        else
        {
            general_message(FATAL, "Name '%s' is not defined in current scope.", tval); // node->token
        }
        cstring_delete(key);
        parser_index += 1;
    }
    else
    {
        general_message(FATAL, "Not a literal at %d", parser_index);
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

int node_dot_count = 0;

const char *THEN = "then";
const char *ELSE = "else";

// Private, must be only used by tree_to_dot
void node_to_dot_sub(Node *node, FILE *f, int father, int num, const char *overload)
{
    fprintf(f, "        n%d ;\n", num);
    if (node->token.type == TOKEN_KEYWORD)
    {
        fprintf(f, "        n%d [label=\"%.*s\" fontname=\"Helvetica,Arial,sans-serif bold\"]\n", num, node->token.count, node->token.text + node->token.start);
    }
    else if (overload != NULL)
    {
        fprintf(f, "        n%d [label=\"%s\" fontname=\"Helvetica,Arial,sans-serif bold\"]\n", num, overload);
    }
    else
    {
        fprintf(f, "        n%d [label=\"%.*s\"]\n", num, node->token.count, node->token.text + node->token.start);
    }
    // Link to the father node
    if (father != 0)
    {
        fprintf(f, "        n%d -- n%d ;\n", father, num);
    }
    // Go extra
    if (node->extra != NULL)
    {
        node_dot_count += 1;
        node_to_dot_sub(node->extra, f, num, node_dot_count, NULL);
    }
    // Go left
    if (node->left != NULL)
    {
        node_dot_count += 1;
        if (node->type == NODE_IF)
        {
            node_to_dot_sub(node->left, f, num, node_dot_count, THEN);
        }
        else
        {
            node_to_dot_sub(node->left, f, num, node_dot_count, NULL);
        }
    }
    // Go right
    if (node->right != NULL)
    {
        node_dot_count += 1;
        if (node->type == NODE_IF)
        {
            node_to_dot_sub(node->right, f, num, node_dot_count, ELSE);
        }
        else
        {
            node_to_dot_sub(node->right, f, num, node_dot_count, NULL);
        }
    }
}

void ast_to_dot(AST *tree, const char *res)
{
    node_dot_count = 0;
    FILE *f = NULL;
    errno_t err = fopen_s(&f, "output.dot", "w"); //, ccs=UTF-8");
    // f = fopen("output.dot", "w");
    if (err != 0 || f == NULL)
    {
        printf("Error opening file!\n");
        exit(EXIT_FAILURE);
    }
    fprintf(f, "graph \"Output parser\"\n");
    fprintf(f, "{\n");
    fprintf(f, "    fontname=\"Helvetica,Arial,sans-serif\"\n");
    fprintf(f, "    node [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(f, "    label = \"Abstract syntax tree (Result = %s)\"\n", res);
    fprintf(f, "    subgraph s1\n");
    fprintf(f, "    {\n");
    if (tree->root != NULL)
    {
        node_dot_count += 1;
        node_to_dot_sub(tree->root, f, 0, node_dot_count, NULL);
    }
    fprintf(f, "    }\n}\n");
    fclose(f);
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
        printf("BINARY OPERATOR [%s] %.*s\n", VALUE_TYPE_STRING[node->value_type], node->token.count, node->token.text + node->token.start);
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
    else if (node->type == NODE_IDENTIFIER)
    {
        printf("IDENTIFIER [%s] %.*s\n", VALUE_TYPE_STRING[node->value_type], node->token.count, node->token.text + node->token.start);
    }
    else if (node->type == NODE_FUNCTION_CALL)
    {
        printf("FUNCTION CALL [%s]", VALUE_TYPE_STRING[node->value_type]);
        token_print_value(node->left->token);
        printf("\n");
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
        if (node->right != NULL)
        {
            spaces(level + 1);
            printf("ELSE\n");
            node_print_level(node->right, level + 2);
        }
    }
    else if (node->type == NODE_WHILE)
    {
        printf("WHILE\n");
        spaces(level + 1);
        printf("CONDITION\n");
        node_print_level(node->extra, level + 2);
        spaces(level + 1);
        printf("ACTION\n");
        node_print_level(node->left, level + 2);
    }
    else
    {
        general_message(FATAL, "Parser: Unknown node type: %d", node->type);
    }
}

NodeType node_compute_type(Node *node)
{
    if (node->type == NODE_BOOLEAN)
    {
        return NODE_BOOLEAN;
    }
    else if (node->type == NODE_INTEGER)
    {
        return NODE_INTEGER;
    }
    else if (node->type == NODE_FLOAT)
    {
        return NODE_FLOAT;
    }
    else if (node->type == NODE_BINARY_OPERATOR)
    {
        if (token_cmp(node->token, "and") && node_compute_type(node->right) == NODE_BOOLEAN && node_compute_type(node->left) == NODE_BOOLEAN)
        {
            return NODE_BOOLEAN;
        }
        else if (token_cmp(node->token, "or") && node_compute_type(node->right) == NODE_BOOLEAN && node_compute_type(node->left) == NODE_BOOLEAN)
        {
            return NODE_BOOLEAN;
        }
        else if (token_cmp(node->token, ">") || token_cmp(node->token, "<") || token_cmp(node->token, "==") || token_cmp(node->token, "!="))
        {
            return NODE_BOOLEAN;
        }
    }
    return NODE_BLOCK;
}

void ast_print(AST *tree)
{
    node_print_level(tree->root, 0);
}
