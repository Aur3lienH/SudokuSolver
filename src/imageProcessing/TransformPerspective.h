#pragma once
#include "../deepLearning/Matrix.h"
#include "../geometry/Square.h"
#include <stdlib.h> 

Matrix* TransformPerspective(Matrix* in, Square grid, size_t newWidth);