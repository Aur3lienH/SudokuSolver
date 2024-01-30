#pragma once
#include "matrix/Matrix.h"
#include "geometry/Square.h"
#include <stdlib.h> 

Matrix* TransformPerspective(Matrix* in, size_t newWidth, double* h);

Matrix* TransformPerspectiveColor(Matrix* in, Matrix* out, double* h);

Matrix* TransformPerspectiveColor_I(Matrix* in, size_t newWidth, double* h);

//Calculate the homography matrix, necessary for the perspective transformation
double* CalculateH(Square from, Square to);

Square WidthToSquare(size_t width);