@echo off

echo --- Start of script ---

if ["%~1"]==["dynarray"] (
    echo --- Building tests for dynamic array ---
    cl /W4 .\tests\tests_dyn_array.c .\token.c .\general.c /Fe:tests/tests_dyn_array.exe
    del *.obj
    goto end
)
if ["%~1"]==[""] (
    echo [ERROR] No target : choose between :
    echo         - dynarray
)

:end
echo --- End of script ---
