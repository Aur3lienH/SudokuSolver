#pragma once
#include <stdlib.h>
#include <stdio.h>
#include "matrix/Matrix.h"


//Transform a matrix to a digit
Matrix* MatrixToDigit(Matrix* matrix, int* isBlankPtr);

Matrix* Downsize(const Matrix* input, size_t width, size_t height);

void RemoveCorners(Matrix* input, size_t width);

Matrix* ColorCells(const Matrix* input, const size_t size);