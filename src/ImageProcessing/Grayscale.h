#pragma once
#include "../DeepLearning/Matrix.h"
#include <SDL2/SDL.h>
Matrix* GrayscaleToMatrix(SDL_Surface* image);

SDL_Surface* MatrixToSurface(Matrix* matrix);
