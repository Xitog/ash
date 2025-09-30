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
#include "token.h"
#include "general.h"
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include "transpiler_php.h"

//-----------------------------------------------------------------------------
// Constantes
//-----------------------------------------------------------------------------

const char *VERSION = "0.0.69";

#define BUFFER_SIZE 4000
#define COMMAND_SIZE 100

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

void generate_dot(AST *ast, const char * res, const char * output_dot_filename)
{
    general_message(EL_DEBUG, "- Writing dot file -------------------------");
    ast_to_dot(ast, res);
    const size_t command_length = 1024;
    char *command = memory_get(command_length);
    memset(command, '\0', command_length);
    general_message(EL_DEBUG, "DOT file written.\n");
    sprintf_s(command, 1024, "dot -Tpng %s > output.png", output_dot_filename);
    int err = system(command);
    memory_free(command);
    if (err != EXIT_SUCCESS)
    {
        printf("Unable to generate dot and png files.\n");
    }
    general_message(EL_DEBUG, "PNG file written.\n");
}

void execute_file(char *filepath, bool transpile, bool debug, bool dotfile, const char * output_dot_filename)
{
    printf("Trying to open %s\n", filepath);
    bool file_and_buffer = false;
    char *buffer = NULL;
    FILE *file;
    errno_t err = fopen_s(&file, filepath, "rb");
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
            DynArray tda = lex(buffer, true, debug); // we clear spaces
            for (uint32_t count = 0; count < dyn_array_size(tda); count++)
            {
                Token *tok = dyn_array_get(tda, count);
                token_print(*tok);
                printf("\n");
            }
            printf("- Parsing ----------------------------------\n");
            set_display_error(EL_DEBUG);
            parser_set_debug(true);
            parser_init();
            AST *ast = parse(tda);
            printf("- Abstract Syntax Tree ---------------------\n");
            ast_print(ast);
            printf("- Interpreting -----------------------------\n");
            if (transpile)
            {
                transpile_php(ast, "output.php");
                if (dotfile)
                {
                    generate_dot(ast, "", output_dot_filename);
                }
            }
            else
            {
                execute(ast); // const char * res =
            }
            dyn_array_free(&tda);
        }
    }
}

typedef enum
{
    ACTION_UNDEFINED = 0,
    ACTION_TRANSPILE = 1,
    ACTION_DISPLAY_HELP = 2,
    ACTION_DISPLAY_VERSION = 3,
    ACTION_EVAL = 4,
    ACTION_REPL = 5
} Action;

typedef enum
{
    LANG_UNDEFINED = 0,
    LANG_PHP = 1
} Lang;

void token_print_ptr(Token *tok)
{
    token_print(*tok);
}

int main(int argc, char *argv[])
{
    bool debug = false;
    parser_set_debug(debug);
    bool output_json = false;
    bool output_dot = false;
    bool clear_space = true; // only on printing!
    bool do_parsing = true;
    const char *OUTPUT_JSON_FILENAME = "output.json";
    const char *OUTPUT_DOT_FILENAME = "output.dot";
    // Nouvelle façon libre de gérer les arguments
    Lang lang = LANG_UNDEFINED;
    Action action = ACTION_REPL;
    char *target = NULL;
    bool ignore_next = false;
    uint16_t start_of_eval = 0;
    // argv[0] is always ash.exe
    for (uint16_t i = 1; i < argc; i++)
    {
        // Skip the argument of the option -t php
        if (ignore_next)
        {
            ignore_next = false;
            continue;
        }
        if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--transpile") == 0)
        {
            printf("Transpilation\n");
            if (i + 1 >= argc)
            {
                general_message(FATAL, "Option -t / --transpile requires a target language.");
            }
            action = ACTION_TRANSPILE;
            ignore_next = true;
            if (strcmp(argv[i + 1], "php") == 0)
            {
                lang = LANG_PHP;
            }
            else
            {
                general_message(FATAL, "Target language for transpilation not recognized.");
            }
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            printf("Help\n");
            action = ACTION_DISPLAY_HELP;
        }
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
        {
            printf("Version\n");
            action = ACTION_DISPLAY_VERSION;
        }
        else if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--eval") == 0)
        {
            printf("Evaluation\n");
            action = ACTION_EVAL;
            if (i + 1 >= argc)
            {
                general_message(FATAL, "Nothing to eval.");
            }
            start_of_eval = i + 1;
        }
        else if (strcmp(argv[i], "--dot") == 0)
        {
            printf("Dot output\n");
            output_dot = true;
        }
        else if (file_exists(argv[i]))
        {
            printf("File argument\n");
            target = argv[i];
        }
        else
        {
            general_message(FATAL, "Option not handled: %s", argv[i]);
        }
    }
    // Actions
    if (action == ACTION_TRANSPILE)
    {
        if (target == NULL || !file_exists(target))
        {
            general_message(FATAL, "A valid target to transpile must be provided.");
        }
        if (lang == LANG_PHP)
        {
            execute_file(target, true, debug, output_dot, OUTPUT_DOT_FILENAME);
            printf("Calling PHP:\n");
            system("php .\\output.php");
            printf("\n");
        }
        else
        {
            general_message(FATAL, "No target language defined.");
        }
    }
    else if (action == ACTION_DISPLAY_HELP)
    {
        printf("Ash %s help menu:\n", VERSION);
        printf("-h --help: display this help\n");
        // printf("-t --test: run the tests\n");
        printf("-t --transpile lang: transpile to target language\n");
        printf("-e --eval: tokenize a string\n");
        printf("-v --version: display version number\n");
        printf("If no option is provided and one argument is provided, lex the provided file");
        printf("If no option and no argument is provided, start a REPL loop\n");
        printf("Type help in the REPL loop to get the available commands\n");
    }
    else if (action == ACTION_DISPLAY_VERSION)
    {
        printf("Ash %s\n", VERSION);
    }
    else if (action == ACTION_EVAL)
    {
        for (int i = start_of_eval; i < argc; i++)
        {
            printf("Lexing %s:\n", argv[i]);
            DynArray da = lex(argv[i], false, debug);
            printf("Result:\n");
            dyn_array_info(da, token_print_ptr);
        }
    }
    else if (action == ACTION_REPL)
    {
        char *line = memory_get(BUFFER_SIZE);
        memset(line, '\0', BUFFER_SIZE);
        uint32_t index = 0;
        uint32_t count = 0;
        parser_init();
        do
        {
            if (count > 0) // after first command we separate each command by a \n
            {
                line[count + index] = '\n';
                count += 1;
            }
            index += count;
            count = 0;
            int c;
            printf(">>> ");
            while ((c = getchar()) != '\n' && c != EOF && count < COMMAND_SIZE - 1)
            {
                line[count + index] = (char)c;
                count += 1;
            }
            if (strcmp(line + index, "debug") == 0)
            {
                debug = !debug;
                parser_set_debug(debug);
                if (debug)
                {
                    set_display_error(EL_DEBUG);
                    general_message(EL_DEBUG, "debug is ON");
                }
                else
                {
                    set_display_error(LOG);
                    general_message(LOG, "debug is OFF");
                }
            }
            else if (strcmp(line + index, "json") == 0)
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
            else if (strcmp(line + index, "dot") == 0)
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
            else if (strcmp(line + index, "clear") == 0)
            {
                clear_space = !clear_space;
                if (clear_space)
                {
                    printf("Clear spaces from lexer output\n");
                }
                else
                {
                    printf("Keep spaces from lexer output\n");
                }
            }
            else if (strcmp(line + index, "vars") == 0)
            {
                print_root_scope();
            }
            else if (strcmp(line + index, "hist") == 0)
            {
                printf("Command history:\n");
                printf("%s\n", line);
            }
            else if (strcmp(line + index, "parse") == 0)
            {
                do_parsing = !do_parsing;
                if (do_parsing)
                {
                    printf("Parser is ON\n");
                }
                else
                {
                    printf("Parser is OFF\n");
                }
            }
            else if (strcmp(line + index, "help") == 0)
            {
                printf("Ash %s available commands:\n", VERSION);
                printf(
                    "help  : display this help\n"
                    "debug : display debug information\n"
                    "clear : discard blank tokens\n"
                    "json  : export to %s file the last command\n"
                    "dot   : export to %s file the next command\n"
                    "vars  : list variables in root scope\n"
                    "hist  : history of commands\n"
                    "parse : activate or desactivate parsing\n"
                    "exit  : exit the REPL\n",
                    OUTPUT_JSON_FILENAME,
                    OUTPUT_DOT_FILENAME);
            }
            else if (strcmp(line + index, "exit") != 0)
            {
                general_message(EL_DEBUG, "Command : |%s| (#%d)", line + index, count);
                general_message(EL_DEBUG, "- Token list --------------------------------");
                DynArray list = lex(line + index, clear_space, debug);
                if (output_json)
                {
                    printf("- Outputting to JSON ---\n");
                    unsigned int max_length = TOKEN_TYPE_TO_STRING_MAX_LENGTH;
                    // char *buffer = NULL;
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
                    count = 0;
                    for (count = 0; count < dyn_array_size(list); count++)
                    {
                        if (count > 0)
                        {
                            fprintf(file, ",\n");
                        }
                        Token *tok = dyn_array_get(list, count);
                        fprintf(file, "    {");
                        fprintf(file, "        \"start\": %3d,", tok->text.start);
                        fprintf(file, "        \"count\": %3d,", tok->text.length);
                        fprintf(file, "        \"type\": \"%s\",", TOKEN_TYPE_TO_STRING[tok->type]);
                        for (unsigned int j = 0; j < max_length - strlen(TOKEN_TYPE_TO_STRING[tok->type]); j++)
                        {
                            fprintf(file, " ");
                        }
                        fprintf(file, "        \"value\": \"%.*s\"", tok->text.length, tok->text.source + tok->text.start);
                        fprintf(file, "}");
                    }
                    fprintf(file, "%s", "\n]\n");
                    fclose(file);
                }
                if (do_parsing && dyn_array_size(list) > 0)
                {
                    general_message(EL_DEBUG, "- Parsing ----------------------------------");
                    AST *ast = parse(list);
                    general_message(EL_DEBUG, "- Abstract Syntax Tree ---------------------");
                    if (debug)
                    {
                        ast_print(ast);
                    }
                    general_message(EL_DEBUG, "- Interpreting -----------------------------");
                    const char *res = execute(ast);
                    if (output_dot)
                    {
                        generate_dot(ast, res, OUTPUT_DOT_FILENAME);
                        output_dot = false;
                    }
                }
                dyn_array_free(&list);
            }
        } while (strcmp(line + index, "exit") != 0);
        fflush(stdout);
        memory_free(line);
    }
    else
    {
        general_message(FATAL, "No action defined.");
    }
    // read_utf8("data.txt");
    // printf("Unknown command or impossible to open file %s\n", argv[1]);
    memory_summary();
    printf("End of program Ash v%s.\n", VERSION);
    return EXIT_SUCCESS;
}
