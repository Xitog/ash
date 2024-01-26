#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void lex(const char * cmd);
unsigned int read_identifier(const char * cmd, unsigned int start);
unsigned int read_digit(const char * cmd, unsigned int start);
unsigned int read_space(const char * cmd, unsigned int start);

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

const char * IDENTIFIER = "IDENTIFIER";
const char * SPACE = "SPACE";
const char * INTEGER = "INTEGER";

void lex(const char * cmd)
{
    printf("Starting Lexing\n");
    for (unsigned int i = 0; i < strlen(cmd); i++) {
        printf("%d. %c\n", i, cmd[i]);
    }
    unsigned int old = 0;
    unsigned int index = 0;
    unsigned int count = 0;
    char * type = NULL;
    while (index < strlen(cmd)) {
        old = index;
        //printf("SOL: old=%d index=%d length=%llu\n", old, index, strlen(cmd));
        if (isalpha(cmd[index])) {
            count = read_identifier(cmd, index);
            type = (char *) IDENTIFIER;
        } else if (cmd[index] == ' ') {
            count = read_space(cmd, index);
            type = (char *) SPACE;
        } else if (isdigit(cmd[index])) {
            count = read_digit(cmd, index);
            type = (char *) INTEGER;
        }
        index += count;
        //printf("EOL: start=%d index=%d count=%d\n", old, index, count);
        printf("%s (@%d #%d) : |%s|\n", 
                type, old, count, string_sub(cmd, old, count));
    }
}

unsigned int read_space(const char * cmd, unsigned int start)
{
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
    return count;
}

unsigned int read_identifier(const char * cmd, unsigned int start)
{
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
    return count;
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

unsigned int read_digit(const char * cmd, unsigned int start)
{
    unsigned int index = start;
    unsigned int count = 0;
    bool is_prefixed = false;
    bool start_with_zero = false;
    while (index < strlen(cmd)) {
        char c = cmd[index];
        if (isdigit(c)) {
            count += 1;
            if (count == 1 && c == '0') {
                start_with_zero = true;
            }
        } else if (start_with_zero && count == 1 && c == 'x') {
            is_prefixed = true;
            count += 1;
        } else if (is_prefixed && is_hexa(c)) {
            count += 1;
        } else {
            break;
        }
        index += 1;
    }
    return count;
}

