﻿#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const char * VERSION = "0.0.3";

typedef enum {
    DECIMAL = 1,
    HEXADEICMAL = 2,
    BINARY = 3,
    FLOAT = 4,
    IDENTIFIER = 5,
    KEYWORD = 6,
    SPACE = 7,
    OPERATOR = 8,
    NEWLINE = 9,
    SEPARATOR = 10,
    STRING = 11
} Type;

char * REPR[] = {
    "NONE",
    "INTEGER",
    "HEXADECIMAL",
    "BINARY",
    "FLOAT",
    "IDENTIFIER",
    "KEYWORD",
    "SPACE",
    "OPERATOR",
    "NEWLINE",
    "SEPARATOR",
    "STRING"
};

typedef struct {
    Type type;
    unsigned int start;
    unsigned int count;
} Token;

void lex(const char * cmd);
Token read_identifier(const char * cmd, unsigned int start);
Token read_digit(const char * cmd, unsigned int start);
Token read_hexa(const char * cmd, unsigned int start, unsigned int current);
Token read_float(const char * cmd, unsigned int start, unsigned int current);
Token read_space(const char * cmd, unsigned int start);
Token read_string(const char * cmd, unsigned int start);

bool file_exists(const char filepath[])
{
    FILE * file;
    file = fopen(filepath, "r");
    bool exist = false;
    if (file != NULL) {
        fclose(file);
        exist = true;
    }
    return exist;
}

int main(int argc, char * argv[])
{
    printf("Ash %s\n", VERSION);
    unsigned int nb_file = 0;
    bool is_file = false;
    for (int i = 1; i < argc; i++) { // skip ash.exe
        if (file_exists(argv[i])) {
            printf("    Arg #%d: |%s| est un fichier\n", i, argv[i]);
            is_file = true;
            nb_file += 1;
        } else {
            printf("    Arg #%d: |%s|\n", i, argv[i]);
        }
    }
    printf("files/arguments = %d/%d\n", nb_file, argc - 1);
    if (nb_file == 0 && argc > 1) {
        lex(argv[1]);
    } else if (nb_file == 1) {
        bool file_and_buffer = false;
        char * buffer = NULL;
        FILE * file;
        file = fopen(argv[1], "rb");
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        buffer = calloc(size + 1, sizeof(char));
        if (buffer) {
            fseek(file, 0, SEEK_SET);
            fread(buffer, 1, size, file);
            file_and_buffer = true;
        }
        if (file != NULL) {
            fclose(file);
        }
        if (file_and_buffer) {
            printf("%s", buffer);
            lex(buffer);
        }
    }
    printf("End of program Ash v%s.\n", VERSION);
    return EXIT_SUCCESS;
}

char buffer[250];
char * string_sub(const char * cmd, unsigned int start, unsigned int count)
{
    memset(buffer, '\0', 250);
    for (unsigned int index = 0; index < count; index++) {
        buffer[index] = cmd[start + index];
    }
    return buffer;
}

void token_print(Token * t, const char * cmd)
{
    printf("%s (@%d #%d) : |%s|\n",
            REPR[t->type], t->start, t->count,
            string_sub(cmd, t->start, t->count));
}

void lex(const char * cmd)
{
    printf("Starting Lexing\n");
    for (unsigned int i = 0; i < strlen(cmd); i++) {
        printf("    %d. %c\n", i, cmd[i]);
    }
    unsigned int old = 0;
    unsigned int index = 0;
    Token t;
    bool discard = false;
    while (index < strlen(cmd)) {
        old = index;
        discard = false;
        //printf("SOL: old=%d index=%d length=%llu\n", old, index, strlen(cmd));
        if (isalpha(cmd[index])) {
            t = read_identifier(cmd, index);
        } else if (cmd[index] == ' ') {
            t = read_space(cmd, index);
        } else if (isdigit(cmd[index])) {
            t = read_digit(cmd, index);
        } else if (cmd[index] == '.') {
            t.count = 1;
            t.start = index;
            t.type = OPERATOR;
        } else if (cmd[index] == '\n') {
            t.count = 1;
            t.start = index;
            t.type = NEWLINE;
        } else if (cmd[index] == '=') {
            t.count = 1;
            t.start = index;
            t.type = OPERATOR;
        } else if (cmd[index] == '\r') {
            t.count = 1;
            t.start = index;
            t.type = NEWLINE;
            discard = true;
        } else if (cmd[index] == '(' || cmd[index] == ')') {
            t.count = 1;
            t.start = index;
            t.type = SEPARATOR;
        } else if (cmd[index] == '"') {
            t = read_string(cmd, index);
        } else {
            printf("Unknown char at %i: %c | %x\n", index, cmd[index], cmd[index]);
            exit(EXIT_FAILURE);
        }
        index += t.count;
        //printf("EOL: start=%d index=%d count=%d\n", old, index, count);
        if (!discard) {
            token_print(&t, cmd);
        }
    }
}

Token read_space(const char * cmd, unsigned int start)
{
    Token t;
    unsigned int index = start;
    unsigned int count = 0;
    while (index < strlen(cmd)) {
        if (cmd[index] == ' ') {
            count += 1;
        } else {
            break;
        }
        index += 1;
    }
    t.start = start;
    t.count = count;
    t.type = SPACE;
    return t;
}

char * KEYWORDS[] = {
    "if",
    "else",
    "elsif",
    "end",
    "while"
};

#define NB_KEYWORDS 5

bool is_keyword(const char * cmd, Token t)
{
    // Il faut parcourir les keywords et tester si on reconnaît l'un d'entre eux
    bool ok[NB_KEYWORDS];
    for (int i = 0; i < NB_KEYWORDS; i++) {
        if (strlen(KEYWORDS[i]) == t.count) {
            bool ok = true;
            for (int j = 0; j < t.count; j++) {
                if (KEYWORDS[i][j] != cmd[t.start + j]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                return true;
            }
        }
    }
    return false;
}

Token read_identifier(const char * cmd, unsigned int start)
{
    Token t;
    unsigned int index = start;
    unsigned int count = 0;
    while (index < strlen(cmd)) {
        if (isalpha(cmd[index])) {
            count += 1;
        } else {
            break;
        }
        index += 1;
    }
    t.count = count;
    t.start = start;
    t.type = IDENTIFIER;
    if (is_keyword(cmd, t)) {
        t.type = KEYWORD;
    }
    return t;
}

bool is_hexa(char c) {
    bool res;
    if (isdigit(c)) {
        res = true;
    } else {
        switch (c) {
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
                res = true;
                break;
            default:
                res = false;
        }
    }
    return res;
}

Token read_digit(const char * cmd, unsigned int start)
{
    Token t;
    unsigned int index = start;
    unsigned int count = 0;
    if (index < strlen(cmd)
        && cmd[index] == '0'
        && index + 1 < strlen(cmd)
        && cmd[index + 1] == 'x') {
        t = read_hexa(cmd, start, index + 2);
    } else {
        bool is_float = false;
        while (index < strlen(cmd)) {
            char c = cmd[index];
            if (isdigit(c) || c == '_') {
                count += 1;
            } else if (c == '.' && index + 1 < strlen(cmd) && isdigit(cmd[index + 1])) {
                is_float = true;
                t = read_float(cmd, start, index + 1);
                break;
            } else {
                break;
            }
            index += 1;
        }
        if (!is_float) {
            t.count = count;
            t.start = start;
            t.type = DECIMAL;
        }
    }
    return t;
}

Token read_float(const char * cmd, unsigned int start, unsigned int current)
{
    Token t;
    unsigned int index = current;
    unsigned int count = current - start;
    while (index < strlen(cmd)) {
        char c = cmd[index];
        if (isdigit(c)) {
            count += 1;
        } else {
            break;
        }
        index += 1;
    }
    t.count = count;
    t.start = start;
    t.type = FLOAT;
    return t;
}

Token read_hexa(const char * cmd, unsigned int start, unsigned int current)
{
    Token t;
    unsigned int index = current;
    unsigned int count = current - start;
    while (index < strlen(cmd)) {
        char c = cmd[index];
        if (is_hexa(c) || c == '_') {
            count += 1;
        } else {
            break;
        }
        index += 1;
    }
    t.count = count;
    t.start = start;
    t.type = HEXADEICMAL;
    return t;
}

Token read_string(const char * cmd, unsigned int start)
{
    Token t;
    unsigned int index = start + 1; // On presuppose que le premier c'est "
    unsigned int count = 1;
    while (index < strlen(cmd)) {
        char c = cmd[index];
        if (c != '"') {
            count += 1;
        } else {
            count += 1;
            break;
        }
        index += 1;
    }
    t.count = count;
    t.start = start;
    t.type = STRING;
    return t;
}
