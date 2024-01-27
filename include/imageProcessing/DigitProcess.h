#pragma once
#include <stdlib.h>
#include <stdio.h>
#include "matrix/Matrix.h"


//Transform a matrix to a digit
Matrix* MatrixToDigit(Matrix* matrix, int* isBlankPtr);

Matrix* Downsize(const Matrix* input, size_t width, size_t height);


