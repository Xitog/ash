#ifndef __PARSER__
#define __PARSER__

#include "token_list.h"

typedef struct _Node {
    struct _Node * left;
    struct _Node * middle;
    struct _Node * right;
} Node;

typedef struct {
    Node * root;
} Tree;

Node * parse_addition(TokenList * list);
Node * parse_litteral(TokenList * list);
Tree * parse(TokenList * list);

#endif
