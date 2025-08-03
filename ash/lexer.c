#include "lexer.h"

bool char_is(const char c, const char *set)
{
    for (unsigned int i = 0; i < strlen(set); i++)
    {
        if (set[i] == c)
        {
            return true;
        }
    }
    return false;
}

Token read_space(const char *cmd, unsigned int start)
{
    Token t;
    unsigned int index = start;
    unsigned int count = 0;
    while (index < strlen(cmd))
    {
        if (cmd[index] == ' ' || cmd[index] == '\t' || cmd[index] == '\r')
        {
            count += 1;
        }
        else
        {
            break;
        }
        index += 1;
    }
    t.text = cmd;
    t.start = start;
    t.count = count;
    t.type = TOKEN_SPACE;
    t.line = LINE_COUNT;
    return t;
}

Token read_identifier(const char *cmd, unsigned int start)
{
    Token t;
    unsigned int index = start;
    unsigned int count = 0;
    while (index < strlen(cmd))
    {
        if (isalpha(cmd[index]) || isdigit(cmd[index]))
        {
            if (count == 0 && isdigit(cmd[index]))
            {
                general_message(FATAL, "Lexer error: Identifier cannot start by a number.\n");
            }
            count += 1;
        }
        else if (cmd[index] == '?' && ((index + 1) >= strlen(cmd) || (!isalpha(cmd[index + 1]) && cmd[index + 1] != '?')))
        {
            count += 1;
        }
        else
        {
            break;
        }
        index += 1;
    }
    t.text = cmd;
    t.count = count;
    t.start = start;
    t.type = TOKEN_IDENTIFIER;
    t.line = LINE_COUNT;
    if (text_part_cmp(cmd, start, count, NIL))
    {
        t.type = TOKEN_NIL;
        t.line = LINE_COUNT;
    }
    else if (text_part_cmps(cmd, start, count, BOOLEANS, NB_BOOLEANS))
    {
        t.type = TOKEN_BOOLEAN;
        t.line = LINE_COUNT;
    }
    else if (text_part_cmps(cmd, start, count, OPERATORS, NB_OPERATORS))
    {
        t.type = TOKEN_OPERATOR;
        t.line = LINE_COUNT;
    }
    else if (text_part_cmps(cmd, start, count, KEYWORDS, NB_KEYWORDS))
    {
        t.type = TOKEN_KEYWORD;
        t.line = LINE_COUNT;
    }
    return t;
}

Token read_float(const char *cmd, unsigned int start, unsigned int current)
{
    Token t;
    unsigned int index = current;
    unsigned int count = current - start;
    while (index < strlen(cmd))
    {
        char c = cmd[index];
        if (isdigit(c))
        {
            count += 1;
        }
        else
        {
            break;
        }
        index += 1;
    }
    t.text = cmd;
    t.count = count;
    t.start = start;
    t.type = TOKEN_FLOAT;
    t.line = LINE_COUNT;
    return t;
}

Token read_hexa(const char *cmd, unsigned int start, unsigned int current)
{
    Token t;
    unsigned int index = current;
    unsigned int count = current - start;
    while (index < strlen(cmd))
    {
        char c = cmd[index];
        if (char_is(c, HEXADECIMAL_ELEMENTS) || c == '_')
        {
            count += 1;
        }
        else
        {
            break;
        }
        index += 1;
    }
    t.text = cmd;
    t.count = count;
    t.start = start;
    t.type = TOKEN_HEXADECIMAL;
    t.line = LINE_COUNT;
    return t;
}

Token read_binary(const char *cmd, unsigned int start, unsigned int current)
{
    Token t;
    unsigned int index = current;
    unsigned int count = current - start;
    while (index < strlen(cmd))
    {
        char c = cmd[index];
        if (c == '0' || c == '1' || c == '_')
        {
            count += 1;
        }
        else
        {
            break;
        }
        index += 1;
    }
    t.text = cmd;
    t.count = count;
    t.start = start;
    t.type = TOKEN_BINARY;
    t.line = LINE_COUNT;
    return t;
}

Token read_number(const char *cmd, unsigned int start)
{
    Token t = {.text = cmd, .type = TOKEN_NONE, .start = 0, .count = 0, .line = LINE_COUNT};
    unsigned int index = start;
    unsigned int count = 0;
    if (index < strlen(cmd) && cmd[index] == '0' && index + 1 < strlen(cmd) && cmd[index + 1] == 'x')
    {
        t = read_hexa(cmd, start, index + 2);
    }
    else if (index < strlen(cmd) && cmd[index] == '0' && index + 1 < strlen(cmd) && cmd[index + 1] == 'b')
    {
        t = read_binary(cmd, start, index + 2);
    }
    else
    {
        bool is_float = false;
        while (index < strlen(cmd))
        {
            char c = cmd[index];
            if (isdigit(c) || c == '_')
            {
                count += 1;
            }
            else if (c == '.' && index + 1 < strlen(cmd) && isdigit(cmd[index + 1]))
            {
                is_float = true;
                t = read_float(cmd, start, index + 1);
                break;
            }
            else
            {
                break;
            }
            index += 1;
        }
        if (!is_float)
        {
            t.text = cmd;
            t.count = count;
            t.start = start;
            t.type = TOKEN_DECIMAL;
        }
    }
    if (t.start + t.count < strlen(cmd) && isalpha(cmd[t.start + t.count]))
    {
        t.type = TOKEN_ERROR_NUMBER_WITH_LETTER;
    }
    return t;
}

Token read_string(const char *cmd, unsigned int start)
{
    Token t;
    unsigned int index = start + 1; // On presuppose que le premier c'est "
    unsigned int count = 1;
    while (index < strlen(cmd))
    {
        char c = cmd[index];
        if (c != '"')
        {
            count += 1;
        }
        else
        {
            count += 1;
            break;
        }
        index += 1;
    }
    t.text = cmd;
    t.count = count;
    t.start = start;
    t.type = TOKEN_STRING;
    t.line = LINE_COUNT;
    return t;
}

Token read_comment(const char *cmd, unsigned int start)
{
    Token t;
    unsigned int index = start + 2; // On presuppose que les deux premiers c'est --
    unsigned int count = 1;
    while (index < strlen(cmd))
    {
        char c = cmd[index];
        if (c != '\n')
        {
            count += 1;
        }
        else
        {
            count += 1;
            break;
        }
        index += 1;
    }
    t.text = cmd;
    t.count = count;
    t.start = start;
    t.type = TOKEN_COMMENT;
    t.line = LINE_COUNT;
    return t;
}

Token read_operator(const char *cmd, unsigned int start)
{
    Token t;
    char c = cmd[start];
    char n = ' ';
    char nn = ' ';
    if (start < strlen(cmd) - 1)
    {
        n = cmd[start + 1];
    }
    if (start < strlen(cmd) - 2)
    {
        nn = cmd[start + 2];
    }

    // Opérateurs binaires
    //  7 Opérateurs mathématiques  :
    // Caractères : + - * / % < > = ! . ~ ? & | ^

    if (
        (c == '*' && n == '*' && nn == '=') ||
        (c == '/' && n == '/' && nn == '=') ||
        (c == '<' && n == '<' && nn == '=') ||
        (c == '>' && n == '>' && nn == '=') ||
        (c == '?' && n == '?' && nn == '='))
    {
        t.count = 3;
        t.type = TOKEN_OPERATOR;
    }
    else if (
        (c == '*' && n == '*') ||
        (c == '/' && n == '/') ||
        (c == '<' && n == '=') ||
        (c == '>' && n == '=') ||
        (c == '=' && n == '=') ||
        (c == '!' && n == '=') ||
        (c == '+' && n == '=') ||
        (c == '-' && n == '=') ||
        (c == '*' && n == '=') ||
        (c == '/' && n == '=') ||
        (c == '%' && n == '=') ||
        (c == '&' && n == '=') ||
        (c == '|' && n == '=') ||
        (c == '^' && n == '=') ||
        (c == '~' && n == '=') ||
        (c == '<' && n == '<') ||
        (c == '>' && n == '>') ||
        (c == '?' && n == '?') ||
        (c == '.' && n == '.'))
    {
        t.count = 2;
        t.type = TOKEN_OPERATOR;
    }
    else if (c == '+' || c == '-' ||
             c == '*' || c == '/' ||
             c == '%' || c == '=' ||
             c == '.' || c == '<' ||
             c == '<' || c == '>' ||
             c == '&' || c == '|' ||
             c == '^' || c == '~')
    {
        t.count = 1;
        t.type = TOKEN_OPERATOR;
    }
    else
    {
        t.count = 1;
        t.type = TOKEN_ERROR_LONELY_OPERATOR;
    }
    t.text = cmd;
    t.start = start;
    t.line = LINE_COUNT;
    return t;
}

uint32_t LINE_COUNT;

TokenDynArray lex(const char *cmd, bool skip_spaces, bool debug)
{
    LINE_COUNT = 1;
    if (debug)
    {
        printf("Starting Lexing\n");
        for (unsigned int i = 0; i < strlen(cmd); i++)
        {
            printf("    %d. %c\n", i, cmd[i]);
        }
    }
    unsigned int old = 0;
    unsigned int index = 0;
    Token t;
    t.text = cmd;
    bool discard = false;
    TokenDynArray list = token_dyn_array_init();
    unsigned int count = 0;
    while (index < strlen(cmd))
    {
        old = index;
        discard = false;
        // printf("SOL: old=%d index=%d length=%llu\n", old, index, strlen(cmd));
        if (isalpha(cmd[index]))
        {
            t = read_identifier(cmd, index);
        }
        else if (cmd[index] == ' ' || cmd[index] == '\t' || cmd[index] == '\r')
        {
            t = read_space(cmd, index);
        }
        else if (isdigit(cmd[index]))
        {
            t = read_number(cmd, index);
        }
        else if (cmd[index] == '\n')
        {
            t.count = 1;
            t.start = index;
            t.type = TOKEN_NEWLINE;
            t.line = LINE_COUNT;
            LINE_COUNT += 1;
        }
        else if (cmd[index] == '-' && index + 1 < strlen(cmd) && cmd[index + 1] == '-')
        {
            t = read_comment(cmd, index);
        }
        else if (char_is(cmd[index], SEPARATOR_ELEMENTS))
        {
            t.count = 1;
            t.start = index;
            t.type = TOKEN_SEPARATOR;
            t.line = LINE_COUNT;
        }
        else if (char_is(cmd[index], OPERATOR_ELEMENTS))
        {
            t = read_operator(cmd, index);
        }
        else if (cmd[index] == '"')
        {
            t = read_string(cmd, index);
        }
        else
        {
            printf("Unknown char at %i: %c | %x\n", index, cmd[index], cmd[index]);
            exit(EXIT_FAILURE);
        }
        index += t.count;
        // printf("EOL: start=%d index=%d count=%d\n", old, index, count);
        if ((t.type == TOKEN_SPACE && skip_spaces) || t.type == TOKEN_COMMENT)
        {
            discard = true;
        }
        if (!discard)
        {
            count += 1;
            if (debug)
            {
                printf("%03d. : ", count);
                token_print(t);
                printf("\n");
            }
            token_dyn_array_add(&list, t);
        }
    }
    return list;
}
