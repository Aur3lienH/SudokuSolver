#include "Hough.h"
#include "../geometry/Line.h"
#include "../geometry/Point.h"
#include "../geometry/Square.h"
#include <err.h>
#include <math.h>


const float drawThreshold = 0.99f;
const size_t houghThreshold = 130;
const float angleAverageThreshold = 10.0f;
const float distanceAverageThreshold = 5.0f;
const int searchRadiusThreshold = 5;
size_t batchSize = 1;

void drawLine(Matrix* image, Line line, size_t rhoSize)
{
    for (size_t x = 0; x < image->rows; x++)
    {
        for (size_t y = 0; y < image->cols; y++)
        {
            float theta = line.a * M_PI / 180;
            float rho = line.b - rhoSize / 2;
            if (fabsf((float)(rho - (x * cos(theta) + y * sin(theta)))) < drawThreshold)
            {
                image->data[y + x * image->cols] = 1;
            }
        }
    }
}

float normalizeAngleDifference(float angle) {
    angle = fmodf(angle, 360.0f);
    if (angle < 0) angle += 360.0f;
    if (angle > 180) angle = 360.0f - angle;
    return angle;
}

int find(int parent[], int i) {
    if (parent[i] == -1)
        return i;
    return find(parent, parent[i]);
}

void Union(int parent[], int x, int y) {
    int xset = find(parent, x);
    int yset = find(parent, y);
    if(xset != yset)
        parent[xset] = yset;
}



void averageLines(Line** lines, size_t* linesCount) {
    if(*linesCount == 0) return;

    int parent[*linesCount];
    for (size_t i = 0; i < *linesCount; i++) {
        parent[i] = -1;
    }

    // Grouping lines
    for (size_t i = 0; i < *linesCount; i++) {
        for (size_t j = i + 1; j < *linesCount; j++) {
            float angleDiff = normalizeAngleDifference(fabsf((*lines)[i].a - (*lines)[j].a));
            float distanceDiff = fabsf((*lines)[i].b - (*lines)[j].b);
            
            if (angleDiff < angleAverageThreshold && distanceDiff < distanceAverageThreshold) {
                Union(parent, i, j);
            }
        }
    }

    Line* tempLines = (Line*) calloc(*linesCount, sizeof(Line));
    int count[*linesCount];
    memset(count, 0, sizeof(count));

    for (size_t i = 0; i < *linesCount; i++) {
        int root = find(parent, i);
        tempLines[root].a += (*lines)[i].a;
        tempLines[root].b += (*lines)[i].b;
        count[root]++;
    }

    int newCount = 0;
    for (size_t i = 0; i < *linesCount; i++) {
        if (count[i] > 0) {
            tempLines[newCount].a = normalizeAngleDifference(tempLines[i].a / count[i]);
            tempLines[newCount].b = tempLines[i].b / count[i];
            newCount++;
        }
    }

    // Updating the original lines array
    free(*lines);
    *lines = (Line*) realloc(tempLines, newCount * sizeof(Line));
    *linesCount = newCount;
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

int isQuadrilateral(const Matrix* m,Square square, size_t searchRadius)
{

    for (size_t i = 0; i < 4; i++)
    {
        Point a = square.points[i];
        Point b = square.points[(i + 1) % 4];
        if(!P_IsSegmentComplete(m, &a, &b, searchRadius))
        {
            return 0;
        }
    }
    return 1;
}

Point* SortPointByDistance(Point* intersections, size_t intersection_count, Point distancePoint)
{
    Point* sortedPoints = (Point*)malloc(sizeof(Point) * intersection_count);
    if (!sortedPoints) return NULL; // Check for malloc failure

    for (size_t i = 0; i < intersection_count; i++) {
        sortedPoints[i] = intersections[i];
    }

    for (size_t i = 1; i < intersection_count; i++) {
        Point key = sortedPoints[i];
        int j = i - 1;

        // Move elements that are greater than the key to one position ahead
        while (j >= 0 && P_Distance(&sortedPoints[j], &distancePoint) > P_Distance(&key, &distancePoint)) {
            sortedPoints[j + 1] = sortedPoints[j];
            j--;
        }
        sortedPoints[j + 1] = key;
    }

    return sortedPoints;
}

Square GetSquare(Point* intersections,size_t intersectionsCount, Matrix* input)
{
    //Sorted by upper left corner
    Point* sortedUpLeft = SortPointByDistance(intersections,intersectionsCount,(Point){.x = 0, .y = 0});
    Point* sortedUpRigth = SortPointByDistance(intersections,intersectionsCount,(Point){.x = input->cols, .y = 0});
    Point* sortedDownLeft = SortPointByDistance(intersections,intersectionsCount,(Point){.x = 0, .y = input->rows});
    Point* sortedDownRight = SortPointByDistance(intersections,intersectionsCount,(Point){.x = input->cols, .y = input->rows});

    size_t tempBatchSize = batchSize;
    while(tempBatchSize < intersectionsCount)
    {
        printf("Batch size: %zu\n", tempBatchSize);
        for (size_t i = 0; i < tempBatchSize; i++)
        {
            Point upLeft = sortedUpLeft[i];
            for (size_t j = 0; j < tempBatchSize; j++)
            {
                if(P_Equals(&upLeft, &sortedUpRigth[j]))
                {
                    continue;
                }
                Point upRight = sortedUpRigth[j];
                for (size_t k = 0; k < tempBatchSize; k++)
                {
                    if(P_Equals(&upLeft, &sortedDownLeft[k]) || P_Equals(&upRight, &sortedDownLeft[k]))
                    {
                        continue;
                    }
                    Point downLeft = sortedDownLeft[k];
                    for (size_t l = 0; l < tempBatchSize; l++)
                    {
                        if(P_Equals(&upLeft, &sortedDownRight[l]) || P_Equals(&upRight, &sortedDownRight[l]) || P_Equals(&downLeft, &sortedDownRight[l]))
                        {
                            continue;
                        }
                        Point downRight = sortedDownRight[l];

                        Square square;
                        square.points[0] = upLeft;
                        square.points[1] = upRight;
                        square.points[2] = downRight;
                        square.points[3] = downLeft;
                        if(isQuadrilateral(input, square, searchRadiusThreshold))
                        {
                            return square;
                        }
                    }
                    
                }
                
            }
            
        }
        tempBatchSize = tempBatchSize + 2 > intersectionsCount ? intersectionsCount : tempBatchSize + 2;
    }
    
    errx(1,"No square found");
    
}

Point GetIntersection(Line line1, Line line2, size_t rhoSize)
{
    line1.a = line1.a * M_PI / 180;
    line2.a = line2.a * M_PI / 180;
    line1.b -= rhoSize / 2;
    line2.b -= rhoSize / 2;
    float det = cos(line1.a) * sin(line2.a) - sin(line1.a) * cos(line2.a);
    if (fabs(det) < 1e-6) // Check for zero with a small threshold
    {
        return (Point) {-1, -1}; // No intersection
    }
    float x = (line1.b * sin(line2.a) - line2.b * sin(line1.a)) / det;
    float y = (-line1.b * cos(line2.a) + line2.b * cos(line1.a)) / det;
    return (Point) {y, x};
}


Point* GetIntersectionPoints(Line* lines, size_t linesCount, size_t* interSectionsCount, const Matrix* img, size_t rhoSize)
{
    size_t pointIndex = 0;
    size_t pointSize = 10; // Initial allocation size
    Point* points = (Point*) malloc(pointSize * sizeof(Point));
    if (!points) return NULL; // Check for malloc failure

    for (size_t i = 0; i < linesCount; i++)
    {
        for (size_t j = 0; j < linesCount; j++) // Avoid duplicate calculations
        {
            if(i == j)
            {
                continue;
            }
            Point point = GetIntersection(lines[i], lines[j],rhoSize);
            if (point.x >= 0 && point.y >= 0 && point.x < img->cols && point.y < img->rows)
            {
                if (pointIndex == pointSize)
                {
                    pointSize *= 2;
                    Point* tempPoints = (Point*) realloc(points, pointSize * sizeof(Point));
                    if (!tempPoints) 
                    {
                        free(points); // Free memory on error
                        return NULL;
                    }
                    points = tempPoints;
                }
                points[pointIndex++] = point;
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
    size_t linesCount = 0;
    size_t houghThresholdAdjust = houghThreshold * ((img->cols * img->rows) / (500.0f * 500.0f));

    Line* lines = GetLines(accumulator, thetaSize, rhoSize, houghThresholdAdjust, &linesCount);
    averageLines(&lines, &linesCount);
        //drawLines(img, lines, linesCount, rhoSize);
    


    printf("Lines count: %zu\n", linesCount);
    size_t intersectionsCount = 0;
    Point* intersections = GetIntersectionPoints(lines, linesCount, &intersectionsCount, img, rhoSize);
    
    printf("Intersections count: %zu\n", intersectionsCount);
    //size_t squaresCount = 0;
    
    Square square = GetSquare(intersections, intersectionsCount, img);
    //M_Zero(img);
    //drawLines(img, lines, linesCount, rhoSize);
    //S_Draw(img, &square,1);
    return square;
}

