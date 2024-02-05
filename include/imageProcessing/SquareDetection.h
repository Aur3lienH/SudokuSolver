#pragma once
#include "../geometry/Square.h"
#include "../geometry/Point.h"




typedef struct PointSet
{
    Point* points;
    size_t size;
} PointSet;

typedef struct {
    PointSet* pointSet;
    Square square;
} SquareDetectionResult;

SquareDetectionResult GetSquareWithContour(Matrix* img);

