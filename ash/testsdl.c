#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "..\..\tallentaa\librairies\SDL_2_0_4\include\SDL.h"

const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 200;

#undef main
int main(int argc, char *argv[])
{
    SDL_Window *window = NULL;
    for (int i = 0; i < argc; i++)
    {
        printf("%d. %s\n", i + 1, argv[i]);
    }
    printf("hello!\n");
    window = SDL_CreateWindow("hello", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    SDL_Event e;
    bool quit = false;

    SDL_Surface *screenSurface = SDL_GetWindowSurface(window);
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

    while (quit == false)
    {
        SDL_UpdateWindowSurface(window);
        // SDL_Delay(10000);
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                quit = true;
        }
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
    printf("goodbye!\n");
    return EXIT_SUCCESS;
}
