@echo off

if ["%~1"]==[""] (
    cl /W4 .\main.c .\general.c .\token.c .\value.c .\list.c .\dict.c .\token_list.c .\lexer.c .\parser.c .\interpreter.c .\lua54.lib
    goto end
)
if ["%~1"]==["tests"] (
    echo --- Building tests ---
    cl /W4 .\tests.c .\general.c .\token.c .\value.c .\list.c .\dict.c /Fe:tests.exe
)

:end

del *.obj
