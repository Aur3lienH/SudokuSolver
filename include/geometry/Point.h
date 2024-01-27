#pragma once
#include "matrix/Matrix.h"

#ifndef __ANDROID__
#include "SDL2/SDL.h"
#endif

/*

Represent a 2D point

*/


typedef struct Point
{
    int x;
    int y;
} Point;



Point* P_Create(int x, int y);

float P_Distance(Point* p, Point* other);

void P_DrawSegment(Matrix* matrix, Point* p1, Point* p2, float value);

int P_IsSegmentComplete(const Matrix* m, Point* a, Point* b, int searchRadius);

int P_Equals(Point* p1, Point* p2);

Point P_Add(Point* p1, Point* p2);

void P_DrawSDL(void* image, Point* p, Uint32 color);

void P_Free(Point* point);