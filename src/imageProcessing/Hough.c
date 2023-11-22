#include "Hough.h"
#include "../geometry/Line.h"
#include "../geometry/Point.h"
#include "../geometry/Square.h"
#include <math.h>


const float drawThreshold = 0.99f;
const size_t houghThreshold = 135;
const float angleAverageThreshold = 10.0f;
const float distanceAverageThreshold = 20.0f;
size_t batchSize = 5;

void drawLine(Matrix* image, Line line, size_t rhoSize)
{
    for (size_t x = 0; x < image->rows; x++)
    {
        for (size_t y = 0; y < image->cols; y++)
        {
            float theta = line.a * M_PI / 180;
            float rho = line.b - rhoSize / 2;
            if (fabsf(rho - (x * cos(theta) + y * sin(theta))) < drawThreshold)
            {
                image->data[y + x * image->cols] = 1;
            }
        }
    }
}

void averageLines(Line* lines, size_t* linesCount)
{
    for (size_t i = 0; i < *linesCount; i++)
    {
        for (size_t j = 0; j < *linesCount; j++)
        {
            if(i == j)
            {
                continue;
            }
            if (fabsf(lines[i].a - lines[j].a) < angleAverageThreshold && fabsf(lines[i].b - lines[j].b) < distanceAverageThreshold)
            {
                lines[i].a = (lines[i].a + lines[j].a) / 2;
                lines[i].b = (lines[i].b + lines[j].b) / 2;
                lines[j] = lines[(*linesCount) - 1];
                (*linesCount)--;
            }
        }
    }
}


void drawLines(Matrix* image, Line* lines, size_t linesCount, size_t rhoSize)
{
    for (size_t i = 0; i < linesCount; i++)
    {
        drawLine(image, lines[i], rhoSize);
    }
}

int IsSquare(Square square, float threshold)
{
    float distance1 = P_Distance(&square.points[0], &square.points[1]);
    float distance2 = P_Distance(&square.points[1], &square.points[2]);
    if(distance1 - distance2 > threshold || distance1 - distance2 < -threshold)
    {
        return 0;
    }
    float distance3 = P_Distance(&square.points[2], &square.points[3]);
    float distance4 = P_Distance(&square.points[3], &square.points[0]);
    float distance5 = P_Distance(&square.points[0], &square.points[2]);
    float distance6 = P_Distance(&square.points[1], &square.points[3]);

    return (distance1 - distance2 < threshold && distance1 - distance2 > -threshold) &&
           (distance2 - distance3 < threshold && distance2 - distance3 > -threshold) &&
           (distance3 - distance4 < threshold && distance3 - distance4 > -threshold) &&
           (distance4 - distance1 < threshold && distance4 - distance1 > -threshold) &&
           (distance5 - distance6 < threshold && distance5 - distance6 > -threshold);
}

Square* GetSquares(Point* intersections, size_t intersectionsCount, size_t* squaresCount)
{
    size_t iterations = 0;
    
}

Point GetIntersection(Line line1, Line line2)
{
    float det = cos(line1.a) * sin(line2.b) - cos(line2.b) * sin(line1.a);
    if (det == 0)
    {
        return (Point) {-1, -1};
    }
    float x = (line2.b * sin(line1.a) - line1.b * sin(line2.a)) / det;
    float y = (line1.a * cos(line2.b) - line2.a * cos(line1.b)) / det;
    return (Point) {x, y};
}


Point* GetIntersectionPoints(Line* lines, size_t linesCount, size_t* interSectionsCount, size_t width, size_t height)
{
    size_t pointIndex = 0;
    size_t pointSize = 1;
    Point* points = (Point*) malloc(sizeof(Point));
    for (size_t i = 0; i < linesCount; i++)
    {
        for (size_t j = 0; j < linesCount; j++)
        {
            Point point = GetIntersection(lines[i], lines[j]);
            if (point.x >= 0 && point.y >= 0 && point.x < width && point.y < height)
            {
                points[pointIndex] = point;
                pointIndex++;
                if (pointIndex == pointSize)
                {
                    pointSize *= 2;
                    points = (Point*) realloc(points, pointSize * sizeof(Point));
                }
            }
        }
    }
    *interSectionsCount = pointIndex;
    return points;
}

//Need to adjust the thresold because the image has not the same size everytime
Line* GetLines(unsigned int** accumulator, size_t thetaSize, size_t rhoSize, size_t threshold, size_t* linesCount)
{
    Line* lines = (Line*) malloc(sizeof(Line));
    size_t linesSize = 1;
    size_t index = 0;
    for (size_t theta = 0; theta < thetaSize; theta++)
    {
        for (size_t rho = 0; rho < rhoSize; rho++)
        {
            if (accumulator[theta][rho] >= threshold)
            {
                lines[index] = (Line) {theta, rho};
                index++;
                if(index == linesSize)
                {
                    linesSize *= 2;
                    lines = (Line*) realloc(lines, linesSize * sizeof(Line));
                }
            }
        }
    }
    *linesCount = index;
    return lines;
}


unsigned int** AccumulatorArray(Matrix* image, size_t thetaSize, size_t rhoSize)
{
    unsigned int** accumulator = (unsigned int**) malloc(thetaSize * sizeof(unsigned int*));
    for (size_t i = 0; i < thetaSize; i++)
    {
        accumulator[i] = (unsigned int*) calloc(rhoSize, sizeof(unsigned int));
    }
    for (size_t x = 0; x < image->rows; x++)
    {
        for (size_t y = 0; y < image->cols; y++)
        {
            if (image->data[y + x * image->cols] >= 0.99)
            {
                for (size_t theta = 0; theta < thetaSize; theta++)
                {
                    double thetaRad = (double) theta * M_PI / 180;
                    double rho = x * cos(thetaRad) + y * sin(thetaRad);
                    accumulator[theta][(size_t)(rho + rhoSize/2)]++;
                }
            }
        }
    }
    return accumulator;
}


Square Hough(Matrix* img)
{
    size_t thetaSize = 180;
    size_t rhoSize = (size_t) (sqrt(img->cols*img->cols + img->rows*img->rows) * 2);
    unsigned int** accumulator = AccumulatorArray(img, thetaSize, rhoSize);
    printf("Accumulator created\n");
    size_t linesCount = 0;
    Line* lines = GetLines(accumulator, thetaSize, rhoSize, houghThreshold, &linesCount);
    
    averageLines(lines, &linesCount);
    averageLines(lines, &linesCount);
    printf("Lines count: %zu\n", linesCount);
    drawLines(img, lines, linesCount, rhoSize);
    size_t intersectionsCount = 0;
    Point* intersections = GetIntersectionPoints(lines, linesCount, &intersectionsCount, img->cols, img->rows);
    printf("Intersections count: %zu\n", intersectionsCount);
    //size_t squaresCount = 0;
    
    //Square* squares = GetSquares(intersections, intersectionsCount, &squaresCount);
    Square square;
    return square;
    
}