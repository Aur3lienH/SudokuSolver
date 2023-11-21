#pragma once
#include "../deepLearning/Matrix.h"

typedef struct Point
{
    int x;
    int y;
} Point;


Point* P_Create(int x, int y);

float P_Distance(Point* p, Point* other);

void DrawSegment(Matrix* matrix, Point* p1, Point* p2, float value);

int P_Equals(Point* p1, Point* p2);

Point P_Add(Point* p1, Point* p2);

void P_Free(Point* point);