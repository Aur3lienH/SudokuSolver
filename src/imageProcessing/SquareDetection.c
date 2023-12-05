#include "SquareDetection.h"
#include "../geometry/Point.h"
#include "../geometry/Square.h"
#include "../deepLearning/Matrix.h"


const int PixelCountSkip = 0;

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


void addToStack(Point* stack,int* stackFlag, size_t* stackPos, Point point, int flag)
{
    stack[*stackPos] = point;
    stackFlag[*stackPos] = flag;
    *stackPos += 1;
}

void addPointToStack(Point* stack, int* flagStack, size_t* stackPos, Point point, Matrix* img, Matrix* flag, int skippedPixels) {
    if(point.x < 0 || point.y < 0 || point.x >= img->cols || point.y >= img->rows) {
        return;
    }
    if(flag->data[point.x + point.y * flag->cols] >= 0.9f) {
        return;
    }

    float currentPixelValue = img->data[point.x + point.y * img->cols];

    // If current pixel is black, increment the skippedPixels counter
    if(currentPixelValue <= 0.1f) {
        if(skippedPixels >= PixelCountSkip) {
            return; // Stop skipping if maximum skipped pixel count is reached
        }
        skippedPixels++; // Increment the count of skipped pixels
    } else {
        skippedPixels = 0; // Reset skipped pixel count for non-black pixels
    }

    // Add current point to the stack
    flag->data[point.x + point.y * flag->cols] = 1.0f;
    addToStack(stack, flagStack, stackPos, point, skippedPixels);
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
    Point* stack = malloc(sizeof(Point) * img->rows * img->effectiveCols* 10);
    int* stackFlag = malloc(sizeof(int) * img->rows * img->effectiveCols* 10);
    PointSet* pointSet = malloc(sizeof(PointSet));
    size_t pointSetPos = 0;
    pointSet->points = malloc(sizeof(Point) * 5);
    pointSet->size = 5;
    size_t stackPos = 0;
    Point minUpLeft = startPoint;
    Point minUpRight = startPoint;
    Point minDownLeft = startPoint;
    Point minDownRight = startPoint;

    flag->data[startPoint.x + startPoint.y * flag->cols] = 1.0f;
    Point upLeft = {0,0};
    Point upRight = {img->rows,0};
    Point downLeft = {0,img->cols};
    Point downRight = {img->rows,img->cols};


    addToStack(stack,stackFlag,&stackPos,startPoint,0);
    while(stackPos > 0)
    {
        Point currentPoint = stack[stackPos - 1];
        int flagValue = stackFlag[stackPos - 1];
        stackPos -= 1;

        

        addPointToStack(stack,stackFlag,&stackPos,(Point){currentPoint.x + 1,currentPoint.y},img,flag,flagValue);
        addPointToStack(stack,stackFlag,&stackPos,(Point){currentPoint.x - 1,currentPoint.y},img,flag,flagValue);
        addPointToStack(stack,stackFlag,&stackPos,(Point){currentPoint.x,currentPoint.y + 1},img,flag,flagValue);
        addPointToStack(stack,stackFlag,&stackPos,(Point){currentPoint.x,currentPoint.y - 1},img,flag,flagValue);
        addPointToStack(stack,stackFlag,&stackPos,(Point){currentPoint.x + 1,currentPoint.y + 1},img,flag,flagValue);
        addPointToStack(stack,stackFlag,&stackPos,(Point){currentPoint.x - 1,currentPoint.y - 1},img,flag,flagValue);
        addPointToStack(stack,stackFlag,&stackPos,(Point){currentPoint.x + 1,currentPoint.y - 1},img,flag,flagValue);
        addPointToStack(stack,stackFlag,&stackPos,(Point){currentPoint.x - 1,currentPoint.y + 1},img,flag,flagValue);

        float currentPixelValue = img->data[currentPoint.x + currentPoint.y * img->cols];
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
    free(stackFlag);
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
            if(S_IsSquare(&square,1.0f) == 0)
            {
                continue;
            }
            if(S_Perimeter(&square) > S_Perimeter(&res))
            {
                res = square;
            }
            
            

            
        }
    }
    S_Print(&res);
    return res;
}
