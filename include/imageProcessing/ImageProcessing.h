#pragma once
#include <SDL2/SDL.h>
#include "matrix/Matrix.h"



SDL_Surface* SudokuImgProcessing(SDL_Surface* image);

Matrix** SplitCells(Matrix* image, size_t cellCount);

