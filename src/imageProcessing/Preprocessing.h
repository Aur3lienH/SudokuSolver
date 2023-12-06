#pragma once
#include <SDL2/SDL.h>
#include "../deepLearning/Matrix.h"

Matrix* preprocess(SDL_Surface* image, double factor_size);


Matrix* resize(const Matrix* input, size_t downSizeWidth);
