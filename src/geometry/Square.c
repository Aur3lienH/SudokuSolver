#include "geometry/Square.h"
#include "matrix/Matrix.h"
#include "geometry/Point.h"
#include "imageProcessing/Image.h"
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

void S_DrawSDL(Image* image, Square* square, Uint32 value)
{
    P_DrawSDL(image, &square->points[0], value);
    P_DrawSDL(image, &square->points[1], value);
    P_DrawSDL(image, &square->points[2], value);
    P_DrawSDL(image, &square->points[3], value);
}

int S_IsSquare(Square* square, float threshold)
{
    if (square == NULL)
    {
        return 0; // Invalid input
    }

    float dist[4];
    for (int i = 0; i < 4; i++)
    {
        dist[i] = P_Distance(&square->points[i], &square->points[(i + 1) % 4]);
        if (dist[i] <= 10.0f) 
        {
            return 0; // Side too short to be considered a square
        }
    }

    // Check if all sides are almost equal
    for (int i = 0; i < 4; i++)
    {
        if (dist[i] / dist[(i + 1) % 4] > 1 + threshold || dist[(i + 1) % 4] / dist[i] > 1 + threshold) {
            for (int i = 0; i < 4; i++) 
            {
                if (dist[i] / dist[(i + 1) % 4] > 1.0f + threshold || dist[(i + 1) % 4] / dist[i] > 1.0f + threshold) 
                {
                    return 0;
                }
            }
        }
    }

    float diag1 = P_Distance(&square->points[0], &square->points[2]);
    float diag2 = P_Distance(&square->points[1], &square->points[3]);

    // Check if diagonals are almost equal
    if (diag1 / diag2 > 1 + threshold || diag2 / diag1 > 1 + threshold)
    {
        return 0;
    }
    if (diag1 / diag2 > 1.0f + threshold || diag2 / diag1 > 1.0f + threshold) {
        return 0;
    }

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

int S_IsSquareComplete(Matrix* matrix, Square* square, int searchRadius)
{
    if (square == NULL || square->points == NULL) {
        return 0; // Invalid input
    }

    // Check if all sides are complete
    for (int i = 0; i < 4; i++) 
    {
        if (!P_IsSegmentComplete(matrix, &square->points[i], &square->points[(i + 1) % 4], searchRadius)) {
            return 0;
        }
    }
    return 1; // Passed all checks
}


Point GetClosestUnusedPoint(Point* points, size_t size, Point point, int* used, size_t* index)
{
    Point closestPoint;
    float closestDistance = FLT_MAX;
    for (size_t i = 0; i < size; i++)
    {
        if (!used[i]) {
            float distance = P_Distance(&points[i], &point);
            if (distance < closestDistance)
            {
                closestPoint = points[i];
                *index = i;
                closestDistance = distance;
            }
        }
    }
    return closestPoint;
}

void S_Sort(Square* square, Matrix* img)
{
    // Define the corners of the image
    Point topLeft = {0, 0};
    Point topRight = {img->cols - 1, 0};
    Point bottomLeft = {0, img->rows - 1};
    Point bottomRight = {img->cols - 1, img->rows - 1};

    // Array to keep track of which points have been used
    int used[4] = {0, 0, 0, 0}; // Initialize all to 0 (not used)

    // Find the closest points to each corner of the image
    size_t index;
    Point closestPoints[4];

    closestPoints[0] = GetClosestUnusedPoint(square->points, 4, topLeft,used, &index);
    used[index] = 1; // Mark this point as used

    closestPoints[1] = GetClosestUnusedPoint(square->points, 4, topRight, used, &index);
    used[index] = 1; // Mark this point as used

    closestPoints[2] = GetClosestUnusedPoint(square->points, 4, bottomRight, used, &index);
    used[index] = 1; // Mark this point as used

    // The last point is the one not used
    for (size_t i = 0; i < 4; i++) 
    {
        if (!used[i]) 
        {
            closestPoints[3] = square->points[i];
            break;
        }
    }

    // Reassign the points in the correct order
    for (size_t i = 0; i < 4; i++) 
    {
        square->points[i] = closestPoints[i];
    }

}

