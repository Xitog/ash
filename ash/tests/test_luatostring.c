// Compiled on windows with: cl .\test_luatostring.c ..\lua54.lib
// Tested with lua-5.4.2_Win32_vc17_lib

// Test in CMD on Windows with:
// [1]
// .\test_luatostring.exe """abc"""
// Evaluating: _="abc"
// Type of Res is: string
// Res: abc
// [2]
// .\test_luatostring.exe 2
// Evaluating: _=2
// Type of Res is: number
// Res: 2
// [3]
// .\test_luatostring.exe true
// Evaluating: _=true
// Type of Res is: boolean
// Res: (null)

#include <stdlib.h>
#include <stdio.h>
#include "..\\include\\lua.h"
#include "..\\include\\lualib.h"
#include "..\\include\\lauxlib.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Please provide a string to evaluate.\n");
        return EXIT_FAILURE;
    }
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    char * s = (char *) calloc(1000, sizeof(char));
    strcat(s, "_=");
    strcat(s, argv[1]);
    printf("Evaluating: %s\n", s);
    int res = luaL_dostring(L, s);
    if (res == LUA_OK)
    {
        lua_getglobal(L, "_"); // push onto the stack the value of the global "_"
        int top = lua_gettop(L); // 1
        int type = lua_type (L, top);
        const char * stype = lua_typename(L, type);
        printf("Type of Res is: %s\n", stype);
        const char *msg = lua_tostring(L, top);
        printf("Res: %s\n", msg);
        return EXIT_SUCCESS;
    }
    printf("Lua evaluation failed.\n");
    return EXIT_FAILURE;
}
