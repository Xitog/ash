#ifndef __INTERPRETER__
#define __INTERPRETER__

#include <stdlib.h>
#include <stdio.h>
#include "include\\lua.h"
#include "include\\lualib.h"
#include "include\\lauxlib.h"
#include "general.h"
#include "token.h"
#include "parser.h"

const char * execute(AST *ast);

#endif
