//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------

typedef enum _Type {
    DECIMAL = 1,
    HEXADEICMAL = 2,
    BINARY = 3,
    FLOAT = 4,
    BOOLEAN = 5,
    IDENTIFIER = 6,
    KEYWORD = 7,
    SPACE = 8,
    OPERATOR = 9,
    NEWLINE = 10,
    SEPARATOR = 11,
    STRING = 12
} Type;

typedef struct _Token {
    Type type;
    unsigned int start;
    unsigned int count;
} Token;

//-----------------------------------------------------------------------------
// Constantes
//-----------------------------------------------------------------------------

const char * VERSION = "0.0.5";

char * TYPE_STRING[] = {
    "NONE",
    "INTEGER",
    "HEXADECIMAL",
    "BINARY",
    "FLOAT",
    "BOOLEAN",
    "IDENTIFIER",
    "KEYWORD",
    "SPACE",
    "OPERATOR",
    "NEWLINE",
    "SEPARATOR",
    "STRING"
};

// Liste des caractères qui composent les opérateurs ou les hexadécimaux
const char * OPERATOR_ELEMENTS = "+-/*%=<>!";
const char * HEXADECIMAL_ELEMENTS = "0123456789abcdefABCDEF";
const char * SEPARATOR_ELEMENTS = "([{}])";

char * KEYWORDS[] = {
    "if",
    "else",
    "elsif",
    "end",
    "while"
};

#define NB_KEYWORDS 5

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

bool char_is(const char c, const char * set)
{
    for (int i=0; i < strlen(set); i++) {
        if (set[i] == c) {
            return true;
        }
    }
    return false;
}

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

bool token_cmp(const char * motherstring, const Token t, const char * s)
{
    if (strlen(s) != t.count) {
        return false;
    }
    int i = 0;
    char tc = motherstring[t.start];
    char sc = s[i];
    while (i < t.count) {
        if (tc != sc) {
            return false;
        }
        i+=1;
        tc = motherstring[t.start + i];
        sc = s[i];
    }
    return true;
}

bool is_boolean(const char * cmd, Token t)
{
    return token_cmp(cmd, t, "true") || token_cmp(cmd, t, "false");
}

bool is_keyword(const char * cmd, Token t)
{
    // Il faut parcourir les keywords et tester si on reconnaît l'un d'entre eux
    bool ok[NB_KEYWORDS];
    for (int i = 0; i < NB_KEYWORDS; i++) {
        if (strlen(KEYWORDS[i]) == t.count) {
            if (token_cmp(cmd, t, KEYWORDS[i])) {
                return true;
            }
        }
    }
    return false;
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
            TYPE_STRING[t->type], t->start, t->count,
            string_sub(cmd, t->start, t->count));
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
    t.type = IDENTIFIER;
    if (is_boolean(cmd, t)) {
        t.type = BOOLEAN;
    } else if (is_keyword(cmd, t)) {
        t.type = KEYWORD;
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
        if (char_is(c, HEXADECIMAL_ELEMENTS) || c == '_') {
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

Token read_operator(const char * cmd, unsigned int start)
{
    Token t;
    char c = cmd[start];
    char n = ' ';
    char nn = ' ';
    if (start < strlen(cmd) - 1) {
        n = cmd[start + 1];
    }
    if (start < strlen(cmd) - 2) {
        nn = cmd[start + 2];
    }
    if (c == '+' && n == '=') {                     // +=
        t.count = 2;
    } else if (c == '-' && n == '=') {              // -=
        t.count = 2;
    } else if (c == '*' && n == '=') {              // *=
        t.count = 2;
    } else if (c == '*' && n == '*') {              // **
        t.count = 2;
    } else if (c == '*' && n == '*' && nn == '=') {  // **=
        t.count = 3;
    } else if (c == '/' && n == '=') {              // /=
        t.count = 2;
    } else if (c == '/' && n == '/' && nn == '=') {  // //=
        t.count = 3;
    } else if (c == '%' && n == '=') {              // %=
        t.count = 2;
    } else if (char_is(c, OPERATOR_ELEMENTS)) {
        t.count = 1;
    }
    t.start = start;
    t.type = OPERATOR;
    return t;
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
        } else if (char_is(cmd[index], SEPARATOR_ELEMENTS)) {
            t.count = 1;
            t.start = index;
            t.type = SEPARATOR;
        } else if (char_is(cmd[index], OPERATOR_ELEMENTS)) {
            t = read_operator(cmd, index);
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

void read_utf8(char * s) {
    FILE * file = fopen(s, "rb");
    uint8_t c;
    uint64_t count = 0;
    while (!feof(file)) {
        int raw = fgetc(file);
        if (raw >= 0 && raw < 255) {
            if (raw < 128) {
                c = (uint8_t) raw;
                count += 1;
                if (c != '\r' && c != '\n') {
                    printf("%04d - %03d - %02X - %c  - 1 byte\n", (int) count, (int) c, c, (char) c);
                } else if (c == '\r') {
                    printf("%04d - %03d - %02X - \\r - 1 byte\n", (int) count, (int) c, c);
                } else if (c == '\n') {
                    printf("%04d - %03d - %02X - \\n - 1 byte\n", (int) count, (int) c, c);
                }
            } else if (raw < 224) {
                printf("2 bytes character\n"); // C3 A9
                fgetc(file);
            } else if (raw < 240) {
                printf("3 bytes character\n");
                fgetc(file);
                fgetc(file);
            } else {
                printf("4 bytes character\n");
                fgetc(file);
                fgetc(file);
                fgetc(file);
            }
        } else if (raw == EOF) { // -1 (ou 255 si on le convertit en unsigned)
            printf("End of file\n");
        } else {
            printf("Error while reading: %d\n", raw);
        }
    }
    fclose(file);
}

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

int main(int argc, char * argv[])
{
    printf("Ash %s\n", VERSION);
    unsigned int nb_file = 0;
    bool is_file = false;
    // argv[0] est toujours ash.exe
    if (argc > 1) {
        if (strcmp(argv[1], "-e") == 0 || strcmp(argv[1], "-eval") == 0) {
            const size_t line_length = 1024;
            char * line = malloc(line_length);
            do {
                memset(line, '\0', line_length);
                int c;
                unsigned short count = 0;
                printf(">>> ");
                while ((c = getchar()) != '\n' && c != EOF && count < line_length - 1) {
                    line[count] = c;
                    count += 1;
                }
                if (strcmp(line, "exit") != 0) {
                    printf("Command : |%s| (#%d)\n", line, count);
                    lex(line);
                }
            } while(strcmp(line, "exit") != 0);
            free(line);
            fflush(stdout);
        } else if (strcmp(argv[1], "-t") == 0) {
            read_utf8("data.txt");
        } else if (strcmp(argv[1], "-f")) {
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
        }
    }
    printf("End of program Ash v%s.\n", VERSION);
    return EXIT_SUCCESS;
}
