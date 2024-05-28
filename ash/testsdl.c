#include <stdlib.h>
#include <stdio.h>
#include "SDL.h"

const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 200;

int main(int argc, char * argv[])
{
    SDL_Window* window = NULL;
    for (int i = 0; i < argc; i++) {
        printf("%d. %s\n", i + 1, argv[i]);
    }
    printf("hello!\n");
    window = SDL_CreateWindow("hello", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Surface* screenSurface = SDL_GetWindowSurface(window);
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
    SDL_UpdateWindowSurface(window);
    SDL_Delay(10000);
    SDL_DestroyWindow(window);
    SDL_Quit();
    printf("goodbye!\n");
    return EXIT_SUCCESS;
}