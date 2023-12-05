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
    if (square == NULL || square->points == NULL) {
        return 0; // Invalid input
    }

    float dist[4];
    for (int i = 0; i < 4; i++) {
        dist[i] = P_Distance(&square->points[i], &square->points[(i + 1) % 4]);
        if (dist[i] <= 10) {
            return 0; // Side too short to be considered a square
        }
    }

    // Check if all sides are almost equal
    for (int i = 0; i < 4; i++) {
        if (dist[i] / dist[(i + 1) % 4] > 1 + threshold || dist[(i + 1) % 4] / dist[i] > 1 + threshold) {
            return 0;
        }
    }

    float diag1 = P_Distance(&square->points[0], &square->points[2]);
    float diag2 = P_Distance(&square->points[1], &square->points[3]);

    // Check if diagonals are almost equal
    if (diag1 / diag2 > 1 + threshold || diag2 / diag1 > 1 + threshold) {
        return 0;
    }

    // Optional: Check for right angles and diagonal bisection at 90 degrees
    // This requires implementing an angle calculation function

    return 1; // Passed all checks
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

float S_Area(Square* square)
{
    float dist1 = P_Distance(&square->points[0], &square->points[1]);
    float dist2 = P_Distance(&square->points[1], &square->points[2]);
    return dist1 * dist2;
}

float S_Perimeter(Square* square)
{
    float dist1 = P_Distance(&square->points[0], &square->points[1]);
    float dist2 = P_Distance(&square->points[1], &square->points[2]);
    float dist3 = P_Distance(&square->points[2], &square->points[3]);
    float dist4 = P_Distance(&square->points[3], &square->points[0]);
    return dist1 + dist2 + dist3 + dist4;
}