#pragma once
#include <SDL2/SDL.h>
#include "matrix/Matrix.h"



Matrix* resize(const Matrix* input, size_t downSizeWidth);

Matrix* PreprocessToCanny(SDL_Surface* input, size_t downSizeWidth);

Matrix* PreprocessSquare(Matrix* input);

Matrix* GaussianBlur(Matrix* input,float sigma);