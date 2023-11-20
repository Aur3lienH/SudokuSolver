#include "Point.h"
#include <math.h>
#include "../DeepLearning/Matrix.h"


Point* P_Create(int x, int y)
{
    Point* point = malloc(sizeof(Point));
    point->x = x;
    point->y = y;
    return point;
}

void DrawSegment(Matrix* matrix, Point* p1, Point* p2, float value)
{
    float dx = p2->x - p1->x;
    float dy = p2->y - p1->y;
    float steps = 0;
    if(abs(dx) > abs(dy))
    {
        steps = abs(dx);
    }
    else
    {
        steps = abs(dy);
    }
    float xInc = dx / steps;
    float yInc = dy / steps;
    float x = p1->x;
    float y = p1->y;
    for (size_t i = 0; i < steps; i++)
    {
        size_t x_int = (int)x;
        size_t y_int = (int)y;
        if(x_int >= 0 && x_int < matrix->rows && y_int >= 0 && y_int < matrix->cols)
        {
            matrix->data[x_int * matrix->cols + y_int] = value;
        }
        x += xInc;
        y += yInc;
    }
}

float P_Distance(Point* p, Point* other)
{
    return sqrt(pow(p->x - other->x, 2) + pow(p->y - other->y, 2));
}

Point P_Add(Point* p1, Point* p2)
{
    Point res = {p1->x + p2->x, p1->y + p2->y};
    return res;
}

int P_Equals(Point* p1, Point* p2)
{
    return p1->x == p2->x && p1->y == p2->y;
}


void P_Free(Point* point)
{
    free(point);
}