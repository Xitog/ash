#include "interpreter.h"

char *buffer;
uint32_t current;

// Private function signatures
void emit_token(Token t);
void emit_string(char *s);
void emit_node(Node *node);

void emit_token(Token t)
{
    for (uint32_t i = t.start; i < t.start + t.count; i++, current++)
    {
        buffer[current] = t.text[i];
    }
}

void emit_string(char *s)
{
    for (uint32_t i = 0; i < strlen(s); i++, current++)
    {
        buffer[current] = s[i];
    }
}

void emit_node(Node *node)
{
    if (node == NULL)
    {
        printf("INTERPRETER ERROR");
        return;
    }
    if (node->type == NODE_BINARY_OPERATOR)
    {
        if (node->left->type == NODE_BINARY_OPERATOR)
        {
            // todo
        }
        else if (node->left->type == NODE_BOOLEAN)
        {
            if (token_cmp(node->token, "=="))
            {
                if (node->right->type == NODE_BOOLEAN)
                {
                    emit_node(node->left);
                    emit_string(" == ");
                    emit_node(node->right);
                }
                else
                {
                    general_error("Interpreter: for boolean, equality operator must be used with boolean.");
                }
            }
            else if (token_cmp(node->token, "and"))
            {
                if (node->right->type == NODE_BOOLEAN)
                {
                    emit_node(node->left);
                    emit_string(" and ");
                    emit_node(node->right);
                }
                else
                {
                    general_error("Interpreter: for boolean, and operator must be used with boolean.");
                }
            }
            else if (token_cmp(node->token, "or"))
            {
                if (node->right->type == NODE_BOOLEAN)
                {
                    emit_node(node->left);
                    emit_string(" or ");
                    emit_node(node->right);
                }
                else
                {
                    general_error("Interpreter: for boolean, and operator must be used with boolean.");
                }
            }
            else
            {
                general_error("Interpreter: unknown operator for boolean: %t.", node->token);
            }
        }
        else if (node->left->type == NODE_STRING)
        {
            if (token_cmp(node->token, "+"))
            {
                if (node->right->type == NODE_STRING)
                {
                    emit_node(node->left);
                    emit_string("..");
                    emit_node(node->right);
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
                    emit_string("string.rep(");
                    emit_node(node->left);
                    emit_string(",");
                    emit_node(node->right);
                    emit_string(")");
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
                emit_node(node->left);
                emit_token(node->token);
                emit_node(node->right);
            }
        }
    }
    else if (node->type == NODE_INTEGER || node->type == NODE_FLOAT || node->type == NODE_STRING || node->type == NODE_BOOLEAN)
    {
        emit_token(node->token);
    }
    else if (node->type == NODE_BLOCK)
    {
        if (node->left != NULL)
        {
            emit_node(node->left);
        }
        if (node->right != NULL)
        {
            emit_node(node->right);
        }
    }
    else if (node->type == NODE_IF)
    {
        emit_string("if ");
        emit_node(node->extra);
        emit_string(" then ");
        emit_node(node->left);
        emit_string(" end");
    }
    else if (node->type == NODE_FUNCTION_CALL)
    {
        if (token_cmp(node->left->token, "print"))
        {
            emit_string("print('hello')");
        }
        else
        {
            general_error("Interpreter: unknown function: &t.", node->left->token);
        }
    }
    else
    {
        general_error("Interpreter: Node type unknown.");
    }
}

void execute(Tree *ast)
{
    if (ast == NULL)
    {
        return;
    }
    // Initialisation du buffer de sortie
    buffer = memory_get(sizeof(char) * 1000);
    current = 0;
    bool is_expression = false;
    if (node_is_type(ast->root, NODE_INTEGER) || node_is_type(ast->root, NODE_FLOAT) || node_is_type(ast->root, NODE_STRING) || node_is_type(ast->root, NODE_BOOLEAN) || node_is_type(ast->root, NODE_BINARY_OPERATOR))
    {
        buffer[current] = '_';
        current++;
        buffer[current] = '=';
        current++;
        is_expression = true;
    }
    else if (node_is_type(ast->root, NODE_IF) || node_is_type(ast->root, NODE_BLOCK))
    {
        is_expression = false;
    }
    emit_node(ast->root);
    buffer[current] = '\0';
    printf("Trans> %s\n", buffer);
    // Initialisation et exécution de la VM Lua
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    int res = luaL_dostring(L, buffer);
    if (res == LUA_OK)
    {
        if (is_expression)
        {
            lua_getglobal(L, "_"); // push onto the stack the value of the global "_"
            int top = lua_gettop(L);
            size_t string_size;
            const char *msg = luaL_tolstring(L, top, &string_size);
            printf("Res: %s\n", msg);
        }
        /*
        if (lua_isinteger(L, top) || lua_isstring(L, top))
        {
            const char *msg = lua_tostring(L, top);
            printf("Res: %s\n", msg);
        }
        else if (lua_isboolean(L, top))
        {
            int r = lua_toboolean(L, top);
            if (r == 1)
            {
                printf("Res: true\n");
            }
            else if (r == 0)
            {
                printf("Res: false\n");
            }
            else
            {
                general_error("Interpreter: result is not a boolean.\n");
            }
        }
        else if (lua_isnil(L, top))
        {
            printf("Res: nil\n");
        }
        else
        {
            printf("top: %d\n", top);
            printf("type: %s\n", lua_typename(L, lua_type(L, top)));
        }
        */
    }
    else
    {
        printf("Error: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1); // pop error message
    }
    lua_close(L);
    memory_free(buffer);
}
