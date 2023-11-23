#include "Square.h"
#include "../deepLearning/Matrix.h"
#include "Point.h"
#include <math.h>
#include <stdlib.h>


Square* S_Create(Point* points)
{
    Square* square = malloc(sizeof(Square));
    for (size_t i = 0; i < 4; i++)
    {
        square->points[i] = points[i];
    }
    return square;
}

void S_Draw(Matrix* matrix, Square* square, float value)
{
    P_DrawSegment(matrix, &square->points[0], &square->points[1], value);
    P_DrawSegment(matrix, &square->points[1], &square->points[2], value);
    P_DrawSegment(matrix, &square->points[2], &square->points[3], value);
    P_DrawSegment(matrix, &square->points[3], &square->points[0], value);
}

int S_IsSquare(Square* square, float threshold)
{
    float dist1 = P_Distance(&square->points[0], &square->points[1]);
    float dist2 = P_Distance(&square->points[1], &square->points[2]);
    float dist3 = P_Distance(&square->points[2], &square->points[3]);
    float dist4 = P_Distance(&square->points[3], &square->points[0]);
    float diag1 = P_Distance(&square->points[0], &square->points[2]);
    float diag2 = P_Distance(&square->points[1], &square->points[3]);
    float diff1 = fabs(dist1 - dist2);
    float diff2 = fabs(dist3 - dist4);
    float diff3 = fabs(diag1 - diag2);
    if(diff1 < threshold && diff2 < threshold && diff3 < threshold)
    {
        return 1;
    }
    return 0;
}

void S_Print(Square* square)
{
    printf("Square:\n");
    for (size_t i = 0; i < 4; i++)
    {
        printf("Point %ld: %d, %d\n", i, square->points[i].x, square->points[i].y);
    }
}

void S_Free(Square* square)
{
    free(square);
}