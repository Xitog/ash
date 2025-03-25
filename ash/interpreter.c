#include "interpreter.h"

char * buffer;
uint32_t current;

void emit(Token t)
{
    for (uint32_t i = t.start; i < t.start + t.count; i++, current++)
    {
        buffer[current] = t.text[i];
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
        if (token_cmp(node->token, "+")
            || token_cmp(node->token, "-")
            || token_cmp(node->token, "*")
            || token_cmp(node->token, "/")
            || token_cmp(node->token, "%")) {
            execute_node(node->left);
            emit(node->token);
            execute_node(node->right);
        }
    } else if (node->type == NODE_INTEGER) {
        emit(node->token);
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
