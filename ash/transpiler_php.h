#ifndef TRANSPILER_PHP
#define TRANSPILER_PHP

#include <stdlib.h>
#include <stdio.h>
#include "general.h"
#include "parser.h"

void transpile_php(AST *ast, char *filename);

#endif
