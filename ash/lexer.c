#include "lexer.h"

bool char_is(const char c, const char *set)
{
    for (uint32_t i = 0; i < strlen(set); i++)
    {
        if (set[i] == c)
        {
            return true;
        }
    }
    return false;
}

Token read_space(const char *cmd, uint32_t start)
{
    uint32_t index = start;
    uint32_t count = 0;
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
    TextPart tp = text_part_init(cmd, start, count);
    Token t = token_init(tp, LINE_COUNT, TOKEN_SPACE);
    return t;
}

Token read_identifier(const char *cmd, uint32_t start)
{
    uint32_t index = start;
    uint32_t count = 0;
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
    TextPart tp = text_part_init(cmd, start, count);
    Token t = token_init(tp, LINE_COUNT, TOKEN_IDENTIFIER);
    if (text_part_cmp(tp, NIL))
    {
        t.type = TOKEN_NIL;
    }
    else if (text_part_cmps(tp, BOOLEANS, NB_BOOLEANS))
    {
        t.type = TOKEN_BOOLEAN;
    }
    else if (text_part_cmps(tp, OPERATORS, NB_OPERATORS))
    {
        t.type = TOKEN_OPERATOR;
    }
    else if (text_part_cmps(tp, KEYWORDS, NB_KEYWORDS))
    {
        t.type = TOKEN_KEYWORD;
    }
    return t;
}

Token read_float(const char *cmd, uint32_t start, uint32_t current)
{
    uint32_t index = current;
    uint32_t count = current - start;
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
    TextPart tp = text_part_init(cmd, start, count);
    Token t = token_init(tp, LINE_COUNT, TOKEN_FLOAT);
    return t;
}

Token read_hexa(const char *cmd, uint32_t start, uint32_t current)
{
    uint32_t index = current;
    uint32_t count = current - start;
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
    TextPart tp = text_part_init(cmd, start, count);
    Token t = token_init(tp, LINE_COUNT, TOKEN_HEXADECIMAL);
    return t;
}

Token read_binary(const char *cmd, uint32_t start, uint32_t current)
{
    uint32_t index = current;
    uint32_t count = current - start;
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
    TextPart tp = text_part_init(cmd, start, count);
    Token t = token_init(tp, LINE_COUNT, TOKEN_BINARY);
    return t;
}

Token read_number(const char *cmd, uint32_t start)
{
    Token t;
    uint32_t index = start;
    uint32_t count = 0;
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
        TextPart tp = text_part_init(cmd, start, count);
        t = token_init(tp, LINE_COUNT, TOKEN_FLOAT);
        if (!is_float)
        {
            t.type = TOKEN_DECIMAL;
        }
    }
    if (t.text.start + t.text.length < strlen(cmd) && isalpha(cmd[t.text.start + t.text.length]))
    {
        t.type = TOKEN_ERROR_NUMBER_WITH_LETTER;
    }
    return t;
}

Token read_string(const char *cmd, uint32_t start, char delimiter)
{
    uint32_t index = start + 1; // On presuppose que le premier c'est "
    uint32_t count = 1;
    while (index < strlen(cmd))
    {
        char c = cmd[index];
        if (c != delimiter)
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
    TextPart tp = text_part_init(cmd, start, count);
    Token t = token_init(tp, LINE_COUNT, TOKEN_STRING);
    return t;
}

Token read_comment(const char *cmd, uint32_t start)
{
    uint32_t index = start + 2; // On presuppose que les deux premiers c'est --
    uint32_t count = 2;
    while (index < strlen(cmd) && cmd[index] != '\n')
    {
        count += 1;
        index += 1;
    }
    TextPart tp = text_part_init(cmd, start, count);
    Token t = token_init(tp, LINE_COUNT, TOKEN_COMMENT);
    return t;
}

Token read_operator(const char *cmd, uint32_t start)
{
    uint32_t count = 1;
    TokenType type = TOKEN_OPERATOR;
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
        count = 3;
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
        count = 2;
    }
    else if (c == '+' || c == '-' ||
             c == '*' || c == '/' ||
             c == '%' || c == '=' ||
             c == '.' || c == '<' ||
             c == '<' || c == '>' ||
             c == '&' || c == '|' ||
             c == '^' || c == '~')
    {
        count = 1;
    }
    else
    {
        count = 1;
        type = TOKEN_ERROR_LONELY_OPERATOR;
    }
    TextPart tp = text_part_init(cmd, start, count);
    Token t = token_init(tp, LINE_COUNT, type);
    return t;
}

uint32_t LINE_COUNT;

DynArray lex(const char *cmd, bool skip_spaces, bool debug)
{
    LINE_COUNT = 1;
    if (debug)
    {
        printf("Starting Lexing\n");
        for (uint32_t i = 0; i < strlen(cmd); i++)
        {
            printf("    %d. %c\n", i, cmd[i]);
        }
    }
    uint32_t old = 0;
    uint32_t index = 0;
    Token t;
    t.text.source = cmd;
    bool discard = false;
    DynArray list = dyn_array_init(sizeof(Token));
    uint32_t count = 0;
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
            t.text.length = 1;
            t.text.start = index;
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
            t.text.length = 1;
            t.text.start = index;
            t.type = TOKEN_SEPARATOR;
            t.line = LINE_COUNT;
        }
        else if (char_is(cmd[index], OPERATOR_ELEMENTS))
        {
            t = read_operator(cmd, index);
        }
        else if (cmd[index] == '"' || cmd[index] == '\'')
        {
            t = read_string(cmd, index, cmd[index]);
        }
        else
        {
            printf("Unknown char at %i: %c | %x\n", index, cmd[index], cmd[index]);
            exit(EXIT_FAILURE);
        }
        index += t.text.length;
        // printf("EOL: start=%d index=%d count=%d\n", old, index, count);
        if ((t.type == TOKEN_SPACE && skip_spaces)) // || t.type == TOKEN_COMMENT)
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
            dyn_array_append(&list, &t);
        }
    }
    return list;
}
