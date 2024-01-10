#pragma once
#include <stdlib.h>
#include "matrix/Matrix.h"
#include "../geometry/Point.h"
#include "../geometry/Square.h"

//Return the Square detected by the hough transform
Square Hough(Matrix* img);
