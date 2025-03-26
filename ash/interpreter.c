#include "interpreter.h"

char *buffer;
uint32_t current;

void emit_t(Token t)
{
    for (uint32_t i = t.start; i < t.start + t.count; i++, current++)
    {
        buffer[current] = t.text[i];
    }
}

void emit_s(char *s)
{
    for (uint32_t i = 0; i < strlen(s); i++, current++)
    {
        buffer[current] = s[i];
    }
}

void execute_node(Node *node)
{
    if (node == NULL)
    {
        printf("INTERPRETER ERROR");
        return;
    }
    if (node->type == NODE_BINARY_OPERATOR)
    {
        if (node->left->type == NODE_STRING)
        {
            if (token_cmp(node->token, "+"))
            {
                if (node->right->type == NODE_STRING)
                {
                    execute_node(node->left);
                    emit_s("..");
                    execute_node(node->right);
                }
                else
                {
                    general_error("Interpreter: incompatible operand type for string operator +");
                }
            }
            else if (token_cmp(node->token, "*"))
            {
                if (node->right->type == NODE_INTEGER)
                {
                    emit_s("string.rep(");
                    execute_node(node->left);
                    emit_s(",");
                    execute_node(node->right);
                    emit_s(")");
                }
                else
                {
                    general_error("Interpreter: incompatible operand type for string operator *");
                }
            }
            else
            {
                general_error("Interpreter: unknown operator for string.");
            }
        }
        else
        {
            if (token_cmp(node->token, "+") || token_cmp(node->token, "-") || token_cmp(node->token, "*") || token_cmp(node->token, "/") || token_cmp(node->token, "%") || token_cmp(node->token, "//"))
            {
                execute_node(node->left);
                emit_t(node->token);
                execute_node(node->right);
            }
        }
    }
    else if (node->type == NODE_INTEGER || node->type == NODE_FLOAT || node->type == NODE_STRING)
    {
        emit_t(node->token);
    }
}

void execute(Tree *ast)
{
    if (ast == NULL)
    {
        return;
    }

    buffer = memory_get(sizeof(char) * 1000);
    current = 0;
    buffer[current] = '_';
    current++;
    buffer[current] = '=';
    current++;
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    execute_node(ast->root);
    buffer[current] = '\0';
    printf("Trans> %s\n", buffer);
    int res = luaL_dostring(L, buffer);
    if (res == LUA_OK)
    {
        lua_getglobal(L, "_");
        int top = lua_gettop(L);
        const char *msg = lua_tostring(L, top);
        printf("Res: %s\n", msg);
    }
    lua_close(L);
    memory_free(buffer);
}
