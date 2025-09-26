#include "transpiler_php.h"

FILE *output;

void token_fwrite(Token t)
{
    TextPart tp = t.text;
    fprintf_s(output, "%.*s", tp.length, tp.source + tp.start);
}

void transpile_node(Node *node)
{
    if (node == NULL)
    {
        // nothing is emitted
        return;
    }
    else if (node->type == NODE_BLOCK)
    {
        if (node->left != NULL)
        {
            if (node->left->type == NODE_BINARY_OPERATOR)
            {
                if (!token_cmp(node->left->token, "="))
                {
                    fprintf_s(output, "echo ");
                }
                transpile_node(node->left);
                if (!token_cmp(node->left->token, "="))
                {
                    fprintf_s(output, " . \"\\n\" ");
                }
                fwrite(";\n", 2, 1, output);
            }
            else if (node->left->type == NODE_FUNCTION_CALL)
            {
                transpile_node(node->left);
                fwrite(";\n", 2, 1, output);
            }
            else
            {
                transpile_node(node->left);
            }
        }
        if (node->right != NULL)
        {
            if (node->right->type == NODE_BINARY_OPERATOR)
            {
                if (!token_cmp(node->right->token, "="))
                {
                    fprintf_s(output, "echo ");
                }
                transpile_node(node->right);
                if (!token_cmp(node->right->token, "="))
                {
                    fprintf_s(output, " . \"\\n\" ");
                }
                fwrite(";\n", 2, 1, output);
            }
            else if (node->right->type == NODE_FUNCTION_CALL)
            {
                transpile_node(node->right);
                fwrite(";\n", 2, 1, output);
            }
            else
            {
                transpile_node(node->right);
            }
        }
    }
    else if (node->type == NODE_WHILE)
    {
        fwrite("while (", 7, 1, output);
        transpile_node(node->extra);
        fwrite(") {\n", 4, 1, output);
        transpile_node(node->left);
        fwrite("}\n", 2, 1, output);
    }
    else if (node->type == NODE_FUNCTION_CALL)
    {
        if (token_cmp(node->left->token, "print"))
        {
            fprintf_s(output, "echo \"print\\n\"");
        }
        else if (token_cmp(node->left->token, "hello"))
        {
            fprintf_s(output, "echo \"hello\\n\"");
        }
        else if (token_cmp(node->left->token, "goodbye"))
        {
            fprintf_s(output, "echo \"goodbye\\n\"");
        }
        else if (token_cmp(node->left->token, "read"))
        {
            fprintf_s(output, "fgets(STDIN)");
        }
        else
        {
            general_message(FATAL, "Interpreter: unknown function: %t.", node->left->token);
        }
    }
    else if (node->type == NODE_IDENTIFIER)
    {
        fwrite("$", 1, 1, output);
        token_fwrite(node->token);
    }
    else if (node->type == NODE_BINARY_OPERATOR)
    {
        if (token_cmp(node->token, "+") ||
            token_cmp(node->token, "-") ||
            token_cmp(node->token, "*") ||
            token_cmp(node->token, "/") ||
            token_cmp(node->token, "%") ||
            token_cmp(node->token, "<") ||
            token_cmp(node->token, ">") ||
            token_cmp(node->token, "<=") ||
            token_cmp(node->token, ">="))
        {
            transpile_node(node->left);
            fwrite(" ", 1, 1, output);
            putc(*(node->token.text.source + node->token.text.start), output);
            fwrite(" ", 1, 1, output);
            transpile_node(node->right);
        }
        else if (token_cmp(node->token, "//"))
        {
            fwrite("intdiv(", 7, 1, output);
            transpile_node(node->left);
            fwrite(", ", 2, 1, output);
            transpile_node(node->right);
            fwrite(")", 1, 1, output);
        }
        else if (token_cmp(node->token, "="))
        {
            transpile_node(node->left);
            fwrite(" = ", 3, 1, output);
            transpile_node(node->right);
        }
        else
        {
            general_message(FATAL, "Impossible to handle operator '%$'.", node->token);
        }
    }
    else if (node->type == NODE_INTEGER)
    {
        fwrite(node->token.text.source + node->token.text.start, node->token.text.length, 1, output);
    }
    else
    {
        general_message(FATAL, "Impossible to handle node of type %s in PHP transpiler.", NODE_TYPE_REPR_STRING[node->type]);
    }
}

void transpile_php(AST *ast, char *filename)
{
    errno_t err = fopen_s(&output, filename, "w");
    if (err != 0)
    {
        general_message(FATAL, "Impossible to open %s.", filename);
    }
    char *header = "<?php\n";
    char *footer = "\n\n";
    fwrite(header, strlen(header), 1, output);
    transpile_node(ast->root);
    fwrite(footer, strlen(footer), 1, output);
    fclose(output);
}
