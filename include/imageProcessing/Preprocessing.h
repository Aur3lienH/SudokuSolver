#pragma once
#include "imageProcessing/Image.h"
#include "matrix/Matrix.h"



Matrix* resize(const Matrix* input, size_t downSizeWidth);

Matrix* PreprocessToCanny(const Image* input, size_t downSizeWidth);

Matrix* PreprocessSquare(const Matrix* input);

Matrix* GaussianBlur(const Matrix* input,float sigma);

void StrechSudoku(const Matrix* input);

Matrix* resize3D(const Matrix* input, size_t downSizeWidth);