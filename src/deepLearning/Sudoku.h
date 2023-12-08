#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Network.h"


int GetNumber(Network* n,Matrix *img);

int** GetSudokuNumbers(Network* n, Matrix **img);

void ToSudoku();


