// cl main.c list.c /Fe:ash.exe
// .\main.exe

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include "value.h"
//#include "list.h"
#include "token.h"
#include "token_list.h"
#include "general.h"

//-----------------------------------------------------------------------------
// Constantes
//-----------------------------------------------------------------------------

const char *VERSION = "0.0.54";

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

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

bool file_exists(const char filepath[])
{
    bool exist = false;
    FILE *file;
    errno_t err = fopen_s(&file, filepath, "r");
    if (err != 0)
    {
        if (file)
        {
            fclose(file);
        }
    }
    else
    {
        if (file)
        {
            fclose(file);
            exist = true;
        }
    }
    return exist;
}

bool token_cmp(const char *motherstring, const Token t, const char *s)
{
    if (strlen(s) != t.count)
    {
        return false;
    }
    unsigned int i = 0;
    char tc = motherstring[t.start];
    char sc = s[i];
    while (i < t.count)
    {
        if (tc != sc)
        {
            return false;
        }
        i += 1;
        tc = motherstring[t.start + i];
        sc = s[i];
    }
    return true;
}

bool is_boolean(const char *cmd, Token t)
{
    return token_cmp(cmd, t, "true") || token_cmp(cmd, t, "false");
}

bool is_keyword(const char *cmd, Token t)
{
    // Il faut parcourir les keywords et tester si on reconnaît l'un d'entre eux
    for (int i = 0; i < NB_KEYWORDS; i++)
    {
        if (strlen(KEYWORDS[i]) == t.count)
        {
            if (token_cmp(cmd, t, KEYWORDS[i]))
            {
                return true;
            }
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
        if (cmd[index] == ' ')
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
    return t;
}

Token read_identifier(const char *cmd, unsigned int start)
{
    Token t;
    unsigned int index = start;
    unsigned int count = 0;
    while (index < strlen(cmd))
    {
        if (isalpha(cmd[index]))
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
    if (is_boolean(cmd, t))
    {
        t.type = TOKEN_BOOLEAN;
    }
    else if (is_keyword(cmd, t))
    {
        t.type = TOKEN_KEYWORD;
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
    return t;
}

Token read_digit(const char *cmd, unsigned int start)
{
    Token t = {.text = cmd, .type = TOKEN_NONE, .start = 0, .count = 0};
    unsigned int index = start;
    unsigned int count = 0;
    if (index < strlen(cmd) && cmd[index] == '0' && index + 1 < strlen(cmd) && cmd[index + 1] == 'x')
    {
        t = read_hexa(cmd, start, index + 2);
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
    if (c == '+' && n == '=')
    { // +=
        t.count = 2;
    }
    else if (c == '-' && n == '=')
    { // -=
        t.count = 2;
    }
    else if (c == '*' && n == '=')
    { // *=
        t.count = 2;
    }
    else if (c == '*' && n == '*')
    { // **
        t.count = 2;
    }
    else if (c == '*' && n == '*' && nn == '=')
    { // **=
        t.count = 3;
    }
    else if (c == '/' && n == '=')
    { // /=
        t.count = 2;
    }
    else if (c == '/' && n == '/' && nn == '=')
    { // //=
        t.count = 3;
    }
    else if (c == '%' && n == '=')
    { // %=
        t.count = 2;
    }
    else if (c == '.' && n == '.')
    {
        t.count = 2;
    }
    else if (char_is(c, OPERATOR_ELEMENTS))
    {
        t.count = 1;
    }
    t.text = cmd;
    t.start = start;
    t.type = TOKEN_OPERATOR;
    return t;
}

TokenList *lex(const char *cmd, bool debug)
{
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
    TokenList *list = token_list_init();
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
        else if (cmd[index] == ' ' || cmd[index] == '\t')
        {
            t = read_space(cmd, index);
        }
        else if (isdigit(cmd[index]))
        {
            t = read_digit(cmd, index);
        }
        else if (cmd[index] == '\n')
        {
            t.count = 1;
            t.start = index;
            t.type = TOKEN_NEWLINE;
        }
        else if (char_is(cmd[index], SEPARATOR_ELEMENTS))
        {
            t.count = 1;
            t.start = index;
            t.type = TOKEN_SEPARATOR;
        }
        else if (char_is(cmd[index], OPERATOR_ELEMENTS))
        {
            t = read_operator(cmd, index);
        }
        else if (cmd[index] == '\r')
        {
            t.count = 1;
            t.start = index;
            t.type = TOKEN_NEWLINE;
            discard = true;
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
        count += 1;
        index += t.count;
        // printf("EOL: start=%d index=%d count=%d\n", old, index, count);
        if (!discard)
        {
            if (debug)
            {
                token_print(t);
            }
            Token nt = t;
            token_list_append(list, nt);
        }
    }
    return list;
}

void read_utf8(char *s)
{
    FILE *file = NULL;
    errno_t err = fopen_s(&file, s, "rb");
    if (err != 0)
    {
        printf("Something bad happened");
        if (file)
        {
            fclose(file);
        }
    }
    uint8_t c;
    uint64_t count = 0;
    while (!feof(file))
    {
        int raw = fgetc(file);
        if (raw >= 0 && raw < 255)
        {
            if (raw < 128)
            {
                c = (uint8_t)raw;
                count += 1;
                if (c != '\r' && c != '\n')
                {
                    printf("%04d - %03d - %02X - %c  - 1 byte\n", (int)count, (int)c, c, (char)c);
                }
                else if (c == '\r')
                {
                    printf("%04d - %03d - %02X - \\r - 1 byte\n", (int)count, (int)c, c);
                }
                else if (c == '\n')
                {
                    printf("%04d - %03d - %02X - \\n - 1 byte\n", (int)count, (int)c, c);
                }
            }
            else if (raw < 224)
            {
                printf("2 bytes character\n"); // C3 A9
                fgetc(file);
            }
            else if (raw < 240)
            {
                printf("3 bytes character\n");
                fgetc(file);
                fgetc(file);
            }
            else
            {
                printf("4 bytes character\n");
                fgetc(file);
                fgetc(file);
                fgetc(file);
            }
        }
        else if (raw == EOF)
        { // -1 (ou 255 si on le convertit en unsigned)
            printf("End of file\n");
        }
        else
        {
            printf("Error while reading: %d\n", raw);
        }
    }
    fclose(file);
}

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

bool check(TokenList * list, AshType expected)
{
    if (list == NULL) {
        return false;
    } else if (token_list_is_empty(list)) {
        return false;
    }
    Token t = list->head->token;
    return t.type == expected;
}

/*
Tree * parse(TokenList * list)
{
    Tree * t = (Tree *) memory_get(sizeof(Tree));
    parse_addition(list);
    return t;
}

Node * parse_addition(TokenList * list)
{
    Node * result = (Node *) memory_get(sizeof(Node *));
    result->left = parse_litteral(list);
    // assert "+"
    result->right = parse_litteral(list);
    return result;
}

Node * parse_litteral(TokenList * list)
{
    // assert integer, float
    Node * result = (Node *) memory_get(sizeof(Node *));
    list = list;
    return result;
}
*/

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    printf("Ash %s\n", VERSION);
    bool debug = false;
    bool output_json = false;
    // argv[0] est toujours ash.exe
    if (argc > 1)
    {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
        {
            printf("Ash help menu:\n");
            printf("-h --help: display this help\n");
            printf("-t --test: run the tests\n");
            printf("-e --eval: tokenize a string\n");
            printf("If no option is provided and one argument is provided, lex the provided file");
            printf("If no option and no argument is provided, start a REPL loop\n");
        }
        else if (strcmp(argv[1], "-t") == 0 || strcmp(argv[1], "--test") == 0)
        {
            read_utf8("data.txt");
        }
        else if (strcmp(argv[1], "-e") == 0 || strcmp(argv[1], "--eval") == 0)
        {
            for (int i = 2; i < argc; i++)
            { // skip ash.exe and -e/--eval
                lex(argv[i], debug);
            }
        }
        else if (file_exists(argv[1]))
        {
            printf("Trying to open %s\n", argv[1]);
            bool file_and_buffer = false;
            char *buffer = NULL;
            FILE *file;
            errno_t err = fopen_s(&file, argv[1], "rb");
            if (err != 0)
            {
                printf("Something bad happened\n");
                if (file)
                {
                    fclose(file);
                }
            }
            else
            {
                fseek(file, 0, SEEK_END);
                long size = ftell(file);
                buffer = calloc(size + 1, sizeof(char));
                if (buffer)
                {
                    fseek(file, 0, SEEK_SET);
                    fread(buffer, 1, size, file);
                    file_and_buffer = true;
                }
                if (file != NULL)
                {
                    fclose(file);
                }
                if (file_and_buffer)
                {
                    printf("%s", buffer);
                    TokenList *list = lex(buffer, debug);
                    TokenListElement *current = list->head;
                    uint32_t count = 0;
                    while (current != NULL)
                    {
                        count += 1;
                        Token tok = current->token;
                        token_print(tok);
                        current = current->next;
                    }
                    token_list_free(list);
                }
            }
        }
        else
        {
            printf("Unknown command or impossible to open file %s\n", argv[1]);
        }
    }
    else
    {
        const size_t line_length = 1024;
        char *line = memory_get(line_length);
        do
        {
            memset(line, '\0', line_length);
            int c;
            uint32_t count = 0;
            printf(">>> ");
            while ((c = getchar()) != '\n' && c != EOF && count < line_length - 1)
            {
                line[count] = (char)c;
                count += 1;
            }
            if (strcmp(line, "debug") == 0)
            {
                debug = !debug;
                if (debug)
                {
                    printf("debug set to true\n");
                }
                else
                {
                    printf("debug set to false\n");
                }
            }
            else if (strcmp(line, "json") == 0)
            {
                output_json = !output_json;
                if (output_json)
                {
                    printf("producing json output in out.json\n");
                }
                else
                {
                    printf("no producing json\n");
                }
            }
            else if (strcmp(line, "exit") != 0)
            {
                if (debug)
                {
                    printf("Command : |%s| (#%d)\n", line, count);
                }
                TokenList *list = lex(line, debug);
                TokenListElement *current = list->head;
                count = 0;
                while (current != NULL)
                {
                    count += 1;
                    Token tok = current->token;
                    token_print(tok);
                    current = current->next;
                }
                if (output_json)
                {
                    //char *buffer = NULL;
                    FILE *file;
                    errno_t err = fopen_s(&file, "out.json", "w");
                    if (err != 0)
                    {
                        printf("Something bad happened writing json output file\n");
                        if (file)
                        {
                            fclose(file);
                        }
                    }
                    fprintf(file, "%s", "[\n");
                    current = list->head;
                    count = 0;
                    while (current != NULL)
                    {
                        count += 1;
                        if (count > 1)
                        {
                            fprintf(file, ",\n");
                        }
                        Token tok = current->token;
                        fprintf(file, "    {");
                        fprintf(file, "        \"start\": %d,", tok.start);
                        fprintf(file, "        \"count\": %d,", tok.count);
                        fprintf(file, "        \"type\": \"%s\",", TYPE_REPR_STRING[tok.type]);
                        for (unsigned int j = 0; j < 11 - strlen(TYPE_REPR_STRING[tok.type]); j++)
                        {
                            fprintf(file, " ");
                        }
                        fprintf(file, "        \"value\": \"%.*s\"", tok.count, line + tok.start);
                        fprintf(file, "}");
                        current = current->next;
                    }
                    fprintf(file, "%s", "\n]\n");
                    fclose(file);
                }
                token_list_free(list);
            }
        } while (strcmp(line, "exit") != 0);
        fflush(stdout);
        memory_free(line);
    }
    memory_summary();
    printf("End of program Ash v%s.\n", VERSION);
    return EXIT_SUCCESS;
}
