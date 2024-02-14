#pragma once
#include "matrix/Matrix.h"
#include "geometry/Square.h"
#include <stdlib.h> 

Matrix* TransformPerspective(const Matrix* in, size_t newWidth, const double* h);

Matrix* TransformPerspectiveColor(const Matrix* in, Matrix* out, const double* h);

Matrix* TransformPerspectiveColor_I(const Matrix* in, size_t newWidth, const double* h);

//Calculate the homography matrix, necessary for the perspective transformation
double* CalculateH(Square from, Square to);

Square WidthToSquare(size_t width);