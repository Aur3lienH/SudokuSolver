#pragma once
#include <SDL2/SDL.h>
#include "../deepLearning/Matrix.h"



Matrix* resize(const Matrix* input, size_t downSizeWidth);

Matrix* PreprocessToCanny(SDL_Surface* input, size_t downSizeWidth);

Matrix* PreprocessSquare(Matrix* input);