// cl main.c list.c /Fe:ash.exe
// .\main.exe
// Working with lua-5.4.2_Win32_vc17_lib

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
#include "dict.h"
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

//-----------------------------------------------------------------------------
// Constantes
//-----------------------------------------------------------------------------

const char *VERSION = "0.0.66";

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

bool g_debug = false;

void log(const char * msg)
{
    if (g_debug)
    {
        printf("%s\n", msg);
    }
}

int main(int argc, char *argv[])
{
    printf("Ash %s\n", VERSION);
    bool debug = false;
    parser_set_debug(debug);
    bool output_json = false;
    bool output_dot = false;
    bool clear_space = true; // only on printing!
    bool do_parsing = true;
    const char * OUTPUT_JSON_FILENAME = "output.json";
    const char * OUTPUT_DOT_FILENAME = "output.dot";
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
            printf("Type help in the REPL loop to get the available commands\n");
        }
        else if (strcmp(argv[1], "-t") == 0 || strcmp(argv[1], "--test") == 0)
        {
            read_utf8("data.txt");
        }
        else if (strcmp(argv[1], "-e") == 0 || strcmp(argv[1], "--eval") == 0)
        {
            for (int i = 2; i < argc; i++)
            { // skip ash.exe and -e/--eval
                lex(argv[i], false, debug);
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
                    TokenList *list = lex(buffer, true, debug); // we clear spaces
                    TokenListElement *current = list->head;
                    uint32_t count = 0;
                    while (current != NULL)
                    {
                        count += 1;
                        Token tok = current->token;
                        token_print(tok);
                        printf("\n");
                        current = current->next;
                    }
                    printf("- Parsing ----------------------------------\n");
                    parser_set_debug(true);
                    AST * ast = parse(list);
                    printf("- Abstract Syntax Tree ---------------------\n");
                    ast_print(ast);
                    printf("- Interpreting -----------------------------\n");
                    execute(ast); // const char * res =
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
                parser_set_debug(debug);
                if (debug)
                {
                    printf("debug is ON\n");
                }
                else
                {
                    printf("debug is OFF\n");
                }
            }
            else if (strcmp(line, "json") == 0)
            {
                output_json = !output_json;
                if (output_json)
                {
                    printf("producing json output in %s\n", OUTPUT_JSON_FILENAME);
                }
                else
                {
                    printf("no producing json\n");
                }
            }
            else if (strcmp(line, "dot") == 0)
            {
                output_dot = !output_dot;
                if (output_dot)
                {
                    printf("The next statement will produce a dot output in file %s\n", OUTPUT_DOT_FILENAME);
                }
                else
                {
                    printf("DOT output desactivated.\n");
                }
            }
            else if (strcmp(line, "clear") == 0)
            {
                clear_space = !clear_space;
                if (clear_space)
                {
                    printf("Clear spaces from lexer output\n");
                }
                else{
                    printf("Keep spaces from lexer output\n");
                }
            }
            else if (strcmp(line, "parse") == 0)
            {
                do_parsing = !do_parsing;
                if (do_parsing)
                {
                    printf("Parser is ON\n");
                }
                else{
                    printf("Parser is OFF\n");
                }
            }
            else if (strcmp(line, "help") == 0)
            {
                printf("Ash %s available commands:\n", VERSION);
                printf(
                    "help  : display this help\n"
                    "debug : display debug information\n"
                    "clear : discard blank tokens\n"
                    "json  : export to %s file the last command\n"
                    "dot   : export to %s file the next command\n"
                    "parse : activate or desactivate parsing\n"
                    "exit  : exit the REPL\n",
                    OUTPUT_JSON_FILENAME,
                    OUTPUT_DOT_FILENAME
                );
            }
            else if (strcmp(line, "exit") != 0)
            {
                if (debug)
                {
                    printf("Command : |%s| (#%d)\n", line, count);
                }
                log("- Token list --------------------------------\n");
                TokenList *list = lex(line, clear_space, debug);
                TokenListElement *current = list->head;
                if (debug)
                {
                    count = 0;
                    while (current != NULL)
                    {
                        Token tok = current->token;
                        printf("%03d. %p : ", count, &current->token);
                        token_print(tok);
                        printf("\n");
                        current = current->next;
                        count += 1;
                    }
                }
                if (output_json)
                {
                    printf("- Outputting to JSON ---\n");
                    unsigned int max_length = TOKEN_TYPE_TO_STRING_MAX_LENGTH;
                    //char *buffer = NULL;
                    FILE *file;
                    errno_t err = fopen_s(&file, OUTPUT_JSON_FILENAME, "w");
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
                        fprintf(file, "        \"start\": %3d,", tok.start);
                        fprintf(file, "        \"count\": %3d,", tok.count);
                        fprintf(file, "        \"type\": \"%s\",", TOKEN_TYPE_TO_STRING[tok.type]);
                        for (unsigned int j = 0; j < max_length - strlen(TOKEN_TYPE_TO_STRING[tok.type]); j++)
                        {
                            fprintf(file, " ");
                        }
                        fprintf(file, "        \"value\": \"%.*s\"", tok.count, tok.text + tok.start);
                        fprintf(file, "}");
                        current = current->next;
                    }
                    fprintf(file, "%s", "\n]\n");
                    fclose(file);
                }
                if (do_parsing && token_list_size(list) > 0)
                {
                    log("- Parsing ----------------------------------\n");
                    AST * ast = parse(list);
                    log("- Abstract Syntax Tree ---------------------\n");
                    if (debug)
                    {
                        ast_print(ast);
                    }
                    log("- Interpreting -----------------------------\n");
                    const char * res = execute(ast);
                    if (output_dot)
                    {
                        printf("- Writing dot file -------------------------\n");
                        ast_to_dot(ast, res);
                        const size_t command_length = 1024;
                        char *command = memory_get(command_length);
                        memset(command, '\0', command_length);
                        printf("DOT file written.\n");
                        sprintf_s(command, 1024, "dot -Tpng %s > output.png", OUTPUT_DOT_FILENAME);
                        int err = system(command);
                        memory_free(command);
                        if (err != EXIT_SUCCESS)
                        {
                            printf("Unable to generate dot and png files.\n");
                        }
                        printf("PNG file written.\n");
                        output_dot = false;
                    }
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
