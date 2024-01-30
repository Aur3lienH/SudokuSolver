#pragma once
#include "imageProcessing/Image.h"
#include "matrix/Matrix.h"



Matrix* resize(const Matrix* input, size_t downSizeWidth);

Matrix* PreprocessToCanny(Image* input, size_t downSizeWidth);

Matrix* PreprocessSquare(Matrix* input);

Matrix* GaussianBlur(Matrix* input,float sigma);

void StrechSudoku(Matrix* input);

Matrix* resize3D(const Matrix* input, size_t downSizeWidth);