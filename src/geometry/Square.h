#pragma once
#include "Point.h"
#include "../deepLearning/Matrix.h"


typedef struct Square
{
    Point points[4];
} Square;

Square* S_Create(Point* points);

void S_Draw(Matrix* matrix, Square* square, float value);

int S_IsSquare(Square* square, float threshold);

void S_Print(Square* square);

void S_Free(Square* square);