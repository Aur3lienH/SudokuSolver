#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Network.h"


int GetNumber(Network* n,SDL_Surface *img);

int** GetSudokuNumbers(Network* n, SDL_Surface **img);

void ToSudoku();


