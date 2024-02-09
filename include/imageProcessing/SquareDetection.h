#pragma once
#include "../geometry/Square.h"
#include "../geometry/Point.h"

typedef struct {
    PointSet* pointSet;
    Square square;
} SquareDetectionResult;

SquareDetectionResult GetSquareWithContour(Matrix* img);

