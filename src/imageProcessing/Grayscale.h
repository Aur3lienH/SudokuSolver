#pragma once
#include "../deepLearning/Matrix.h"
#include <SDL2/SDL.h>
Matrix* GrayscaleToMatrix(SDL_Surface* image);

SDL_Surface* MatrixToSurface(Matrix* matrix);
