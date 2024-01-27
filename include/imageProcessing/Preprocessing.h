#pragma once
#include <SDL2/SDL.h>
#include "imageProcessing/Image.h"
#include "matrix/Matrix.h"



Matrix* resize(const Matrix* input, size_t downSizeWidth);

Matrix* PreprocessToCanny(* input, size_t downSizeWidth);

Matrix* PreprocessSquare(Matrix* input);

Matrix* GaussianBlur(Matrix* input,float sigma);

void StrechSudoku(Matrix* input);