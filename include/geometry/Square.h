#pragma once
#include "Point.h"
#include "matrix/Matrix.h"

/*

Represent a square with 4 points

*/


typedef struct Square
{
    Point points[4];
} Square;

Square* S_Create(Point* points);

void S_Draw(Matrix* matrix, Square* square, Color color, int grayscale);

void S_DrawSDL(Image* surface, Square* square, __uint32_t value);

float S_Area(Square* square);

float S_Perimeter(Square* square);

int S_IsSquare(Square* square, float threshold);

int S_IsSquareComplete(Matrix* matrix, Square* square, int searchRadius);

void S_Print(Square* square);

void S_Sort(Square* square, Matrix* img);

void S_Free(Square* square);

void S_Extend(Square* square, float factor, Matrix* img);