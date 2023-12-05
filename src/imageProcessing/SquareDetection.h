#pragma once
#include <SDL2/SDL.h>
#include "../geometry/Square.h"
#include "../geometry/Point.h"

typedef struct PointSet
{
    Point* points;
    size_t size;
} PointSet;

Square GetSquareWithContour(Matrix* img);

