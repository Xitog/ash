@echo off

rem .\make.bat
rem .\main.exe
rem .\tests\make.bat dynarray
rem .\tests\tests_dyn_array.exe

if ["%~1"]==[""] (
    cl /W4 .\main.c .\general.c .\token.c .\value.c .\lexer.c .\parser.c .\interpreter.c .\lua54.lib /Fe:ash.exe
    goto end
)

:end

del *.obj
