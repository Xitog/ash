@echo off

if "%~1"=="" (
    echo No parameters have been provided: aborting.
    echo You must provide the name of source file without .c to compile and SDL2 to use the library.
    echo Example: run.bat main
    exit
)

if "%~2"=="" (
    cl /W4 %~1.c
    %~1.exe
)

if "%~2"=="SDL2" (
    cl /W4 %~1.c /I D:\Perso\Projets\git\tallentaa\librairies\SDL2-2.30.3\include /link /LIBPATH:D:\Perso\Projets\git\tallentaa\librairies\SDL2-2.30.3\lib\x86 SDL2.lib SDL2main.lib shell32.lib /SUBSYSTEM:CONSOLE
)
