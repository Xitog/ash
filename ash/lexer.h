#ifndef __LEXER__
#define __LEXER__

#include <stdbool.h>
#include "general.h"
#include "token.h"
#include "token_list.h"

extern uint32_t LINE_COUNT;

Token read_space(const char *cmd, unsigned int start);
Token read_identifier(const char *cmd, unsigned int start);
Token read_float(const char *cmd, unsigned int start, unsigned int current);
Token read_binary(const char *cmd, unsigned int start, unsigned int current);
Token read_hexa(const char *cmd, unsigned int start, unsigned int current);
Token read_number(const char *cmd, unsigned int start);
Token read_string(const char *cmd, unsigned int start);
Token read_comment(const char *cmd, unsigned int start);
Token read_operator(const char *cmd, unsigned int start);
TokenList *lex(const char *cmd, bool skip_spaces, bool debug);

#endif
