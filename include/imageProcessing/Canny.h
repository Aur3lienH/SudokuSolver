#pragma once
#include "matrix/Matrix.h"

//Apply a gaussian filter and a cannny filter to the image and return the result
Matrix* Canny(Matrix* input, float sigma);

