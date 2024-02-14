#pragma once
#include "matrix/Matrix.h"
#include "imageProcessing/Image.h"


Image* SudokuImgProcessing(const Image* image);

Matrix** SplitCells(const Matrix* image, size_t cellCount);

Matrix* M_Grayscale(const Matrix* image);