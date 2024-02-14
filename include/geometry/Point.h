#pragma once
#include "matrix/Matrix.h"
#include "imageProcessing/Image.h"


/*

Represent a 2D point

*/
typedef struct Point
{
    int x;
    int y;
} Point;


typedef struct PointSet
{
    Point* points;
    size_t size;
} PointSet;





Point* P_Create(int x, int y);

float P_Distance(Point* p, Point* other);

void P_DrawSegment(Matrix* matrix, Point* p1, Point* p2, Color color, int grayscale);

int P_IsSegmentComplete(const Matrix* m, Point* a, Point* b, int searchRadius);

int P_Equals(Point* p1, Point* p2);

Point P_Add(Point* p1, Point* p2);

void P_DrawSDL(Image* image, Point* p, __uint32_t color);

void P_Free(Point* point);

PointSet* P_GetAllPointBetween(Matrix* matrix, Point* p1, Point* p2);

void P_GetPerpendicular(Point* direction);

void P_PointSetDraw(Matrix* image, PointSet* pointSet, Color color, int grayscale);

void P_DrawPixelFrom3D(Matrix* img,size_t fromX, size_t fromY, size_t toX, size_t toY);

void P_DrawPixel(Matrix* img, size_t x, size_t y, Color color);

Color P_GetColor(Matrix* img, size_t x, size_t y);

Point P_Center(Point* points, size_t size);

void P_Mult(Point* p, float factor);

void P_Sub(Point* p1, Point* p2);