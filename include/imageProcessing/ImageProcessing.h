#pragma once
#include "matrix/Matrix.h"
#include "imageProcessing/Image.h"


Image* SudokuImgProcessing(Image* image);

Matrix** SplitCells(Matrix* image, size_t cellCount);

Matrix* M_Grayscale(Matrix* image);