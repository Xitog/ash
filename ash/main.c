#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    LITT_INT_DEC = 1,
    LITT_INT_HEX = 2,
    LITT_INT_BIN = 3,
    LITT_FLT = 4,
    LITT_ID = 5,
    LITT_KW = 6,
    SPACE = 7,
} Type;

char * REPR[] = {
    "NONE",
    "INTEGER",
    "HEXADECIMAL",
    "BINARY",
    "FLOAT",
    "IDENTIFIER",
    "KEYWORD",
    "SPACE"
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
Token read_space(const char * cmd, unsigned int start);

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
    unsigned int nb_file = 0;
    bool is_file = false;
    for (int i = 1; i < argc; i++) { // skip ash.exe
        if (file_exists(argv[i])) {
            printf("%d. %s est un fichier\n", i, argv[i]);
            is_file = true;
            nb_file += 1;
        } else {
            printf("%d. %s\n", i, argv[i]);
        }
    }
    printf("files/arguments = %d/%d\n", nb_file, argc - 1);
    if (nb_file == 0 && argc > 1) {
        lex(argv[1]);
    }
    printf("End of program.\n");
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
        printf("%d. %c\n", i, cmd[i]);
    }
    unsigned int old = 0;
    unsigned int index = 0;
    Token t;
    while (index < strlen(cmd)) {
        old = index;
        //printf("SOL: old=%d index=%d length=%llu\n", old, index, strlen(cmd));
        if (isalpha(cmd[index])) {
            t = read_identifier(cmd, index);
        } else if (cmd[index] == ' ') {
            t = read_space(cmd, index);
        } else if (isdigit(cmd[index])) {
            t = read_digit(cmd, index);
        }
        index += t.count;
        //printf("EOL: start=%d index=%d count=%d\n", old, index, count);
        token_print(&t, cmd);
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
    t.type = LITT_ID;
    return t;
}

bool is_hexa(char c) {
    bool res;
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
        t.type = LITT_INT_DEC;
    }
    return t;
}

Token read_hexa(const char * cmd, unsigned int start, unsigned int current)
{
    Token t;
    unsigned int index = current;
    unsigned int count = current - start;
    while (index < strlen(cmd)) {
        char c = cmd[index];
        if (isdigit(c) || is_hexa(c) || c == '_') {
            count += 1;
        } else {
            break;
        }
        index += 1;
    }
    t.count = count;
    t.start = start;
    t.type = LITT_INT_HEX;
    return t;
}
