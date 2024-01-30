#include "imageProcessing/SquareDetection.h"
#include "geometry/Point.h"
#include "geometry/Square.h"
#include "matrix/Matrix.h"
#include <math.h>

typedef struct {
    Point point;
    Point direction;
    int skippedPixels;
} StackElement;

const int PixelCountSkip = 5;
const int PixelCheckLength = 10;

void RotatePoint(Point* point)
{
    if(point->y == 1 && point->x != 1)
    {
        point->x += 1;
    }
    else if(point->x == 1 && point->y != -1)
    {
        point->y -= 1;
    }
    else if(point->y == -1 && point->x != -1)
    {
        point->x -= 1;
    }
    else if(point->x == -1 && point->y != 1)
    {
        point->y += 1;
    }
}


void addToStack(StackElement* stack,StackElement stackElement, size_t* stackPos)
{
    stack[*stackPos] = stackElement;
    *stackPos += 1;
}



void addPointToStack(StackElement* stack, StackElement stackElement, size_t* stackPos, Matrix* flag, Matrix* img) {

    Point point = stackElement.point;
    Point direction = stackElement.direction;
    int skippedPixels = stackElement.skippedPixels;
    if(point.x < 0 || point.y < 0 || point.x >= img->cols || point.y >= img->rows) {
        return;
    }
    if(flag->data[point.x + point.y * flag->cols] >= 0.9f) {
        return;
    }

    float currentPixelValue = img->data[point.x + point.y * img->cols];
    Point inverseDirection = {direction.x * -1, direction.y * -1};
    int isStraightLineBehind = 1;
    for (size_t i = 1; i < PixelCheckLength + 1 ;i++)
    {
        Point nextPoint = {point.x + inverseDirection.x * i, point.y + inverseDirection.y * i};
        if(nextPoint.x < 0 || nextPoint.y < 0 || nextPoint.x >= img->cols || nextPoint.y >= img->rows) {
            return;
        }
        if(img->data[nextPoint.x + nextPoint.y * img->cols] <= 0.1f && flag->data[nextPoint.x + nextPoint.y * flag->cols] <= 0.1f) {
            isStraightLineBehind = 0;
            break;
        }
    }
    
    
    // If current pixel is black, increment the skippedPixels counter
    if(currentPixelValue <= 0.1f) {
        //Return if the direction is not the same as the previous one
        if(skippedPixels >= PixelCountSkip) {
            return; // Stop skipping if maximum skipped pixel count is reached
        }
        if(isStraightLineBehind == 0)
        {
            return; //
        }
        skippedPixels++; // Increment the count of skipped pixels
    } else {
        skippedPixels = 0; // Reset skipped pixel count for non-black pixels
    }

    // Add current point to the stack
    flag->data[point.x + point.y * flag->cols] = 1.0f;
    addToStack(stack, (StackElement){point, direction, skippedPixels}, stackPos);
}


int isInRangeOrNotFlagged(Matrix* img, Matrix* flag, Point point)
{
    if(point.x < 0 || point.y < 0 || point.x >= img->cols || point.y >= img->rows)
    {
        return 0;
    }
    if(flag->data[point.x + point.y * flag->cols] >= 0.9f)
    {
        return 0;
    }
    return 1;
}

PointSet* GetOneSquare(Matrix* img, Matrix* flag, Point startPoint)
{
    StackElement* stack = malloc(sizeof(StackElement) * img->cols * img->rows);

    //Init the point set for the result
    PointSet* pointSet = malloc(sizeof(PointSet));
    size_t pointSetPos = 0;
    pointSet->points = malloc(sizeof(Point) * 5);
    pointSet->size = 5;

    size_t stackPos = 0;

    flag->data[startPoint.x + startPoint.y * flag->cols] = 1.0f;

    Point upLeft = {0,0};
    Point upRight = {img->rows,0};
    Point downLeft = {0,img->cols};
    Point downRight = {img->rows,img->cols};


    addToStack(stack,(StackElement){startPoint,(Point){1,0},0},&stackPos);
    while(stackPos > 0)
    {
        StackElement currentStackElement = stack[stackPos - 1];
        stackPos -= 1;

        Point currentPoint = currentStackElement.point;
        float currentPixelValue = img->data[currentPoint.x + currentPoint.y * img->cols];
    

        Point direction = currentStackElement.direction;
        Point currentDirection = currentStackElement.direction;
        Point nextPoint;
        for (size_t l = 0; l < 8; l++)
        {
            Point nextPoint = {currentPoint.x + currentDirection.x, currentPoint.y + currentDirection.y};
            addPointToStack(stack, (StackElement){nextPoint, currentDirection, currentStackElement.skippedPixels}, &stackPos, flag, img);
            RotatePoint(&currentDirection);
        }

        if(currentPixelValue <= 0.1f)
        {
            continue;
        }
        else
        {
            pointSet->points[pointSetPos] = currentPoint;
            pointSetPos += 1;
            if(pointSetPos >= pointSet->size)
            {
                pointSet->size *= 2;
                pointSet->points = realloc(pointSet->points,sizeof(Point) * pointSet->size);
            }
        }
        

        





    }
    free(stack);
    pointSet->size = pointSetPos;
    return pointSet;
}

int isSquare(Square square)
{
    //Check if the difference between the distance of the two diagonals is less than 10% of the distance of the diagonals
    float diag1 = P_Distance(&square.points[0],&square.points[2]);
    float diag2 = P_Distance(&square.points[1],&square.points[3]);
    float diff = fabsf(diag1 - diag2);
    float min = diag1 < diag2 ? diag1 : diag2;
    if(diff > min * 1.0f)
    {
        return 0;
    }
    return 1;
}

float distanceFromPoints(Point* points, size_t size, Point point)
{
    if(size == 0)
    {
        Point p = {0,0};
        return P_Distance(&p,&point);
    }
    float sum = 0;
    for (size_t i = 0; i < size; i++)
    {
        float dist = P_Distance(&points[i],&point);
        sum += dist;
    }
    return sum;
}

Square getSquareFromPointSet(PointSet* pointSet, Matrix* img)
{
    Point* points = malloc(sizeof(Point) * 4);
    for (size_t i = 0; i < 4; i++)
    {
        Point newPoint = pointSet->points[0];
        for (size_t j = 0; j < pointSet->size; j++)
        {
            float dist = distanceFromPoints(points,i,pointSet->points[j]);
            if(dist > distanceFromPoints(points,i,newPoint))
            {
                newPoint = pointSet->points[j];
            }
        }
        points[i] = newPoint;
    }
    Square square;
    square.points[0] = points[0];
    square.points[1] = points[1];
    square.points[2] = points[2];
    square.points[3] = points[3];
    free(points); 
    return square;
    
}


Square GetSquareWithContour(Matrix* img)
{
    Matrix* flag = M_Create_2D(img->rows,img->cols);
    Square res;
    res.points[0] = (Point){0,0};
    res.points[1] = (Point){0,0};
    res.points[2] = (Point){0,0};
    res.points[3] = (Point){0,0};

    for (size_t i = 0; i < img->rows; i++)
    {
        for (size_t j = 0; j < img->cols; j++)
        {
            if(img->data[j + i * img->cols] <= 0.1 || flag->data[j + i * flag->cols] >= 0.9)
            {
                continue;
            }
            PointSet* pointSet = GetOneSquare(img,flag,(Point){j,i});
            Square square = getSquareFromPointSet(pointSet,img);
            S_Sort(&square,img);
            if(S_IsSquareComplete(img,&square,5))
            {
                if(S_Perimeter(&square) > S_Perimeter(&res))
                {
                    res = square;
                }
            }

            
        }
    }
    M_SaveImage(flag,"images/export/step_1.jpg");
    S_Sort(&res, img);
    return res;
}
