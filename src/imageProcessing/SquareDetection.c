#include "SquareDetection.h"
#include "../geometry/Point.h"
#include "../geometry/Square.h"
#include "../deepLearning/Matrix.h"




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


void addToStack(Point* stack, size_t* stackPos, Point point)
{
    stack[*stackPos] = point;
    *stackPos += 1;
}

int isInRangeOrNotFlagged(Matrix* img, Matrix* flag, Point point)
{
    if(point.x < 0 || point.y < 0 || point.x >= img->cols || point.y >= img->rows)
    {
        return 0;
    }
    if(flag->data[point.x + point.y * flag->cols] >= 0.9)
    {
        return 0;
    }
    return 1;
}

Square GetOneSquare(Matrix* img, Matrix* flag, Point startPoint)
{
    Point* stack = malloc(sizeof(Point) * img->effectiveRows * img->effectiveCols);
    size_t stackPos = 0;
    Point minUpLeft = {img->cols,img->rows};
    Point minUpRight = {0,img->rows};
    Point minDownLeft = {img->cols,0};
    Point minDownRight = {0,0};

    Point upLeft = {0,0};
    Point upRight = {img->cols,0};
    Point downLeft = {0,img->rows};
    Point downRight = {img->cols,img->rows};

    addToStack(stack,&stackPos,startPoint);
    while(stackPos > 0)
    {
        Point currentPoint = stack[stackPos - 1];
        stackPos -= 1;
        if(currentPoint.x < 0 || currentPoint.y < 0 || currentPoint.x >= img->cols || currentPoint.y >= img->rows)
        {
            printf("Point out of bounds\n");
            continue;
        }
        if(M_Get(img,currentPoint.y,currentPoint.x) == 0)
        {
            printf("Point is not an edge\n");
            continue;
        }
        printf("current point : %ld %ld \n",currentPoint.x,currentPoint.y);
        Point right = {currentPoint.x + 1,currentPoint.y};
        if(isInRangeOrNotFlagged(img,flag,right))
        {
            addToStack(stack,&stackPos,right);
        }
        Point left = {currentPoint.x - 1,currentPoint.y};
        if(isInRangeOrNotFlagged(img,flag,left))
        {
            addToStack(stack,&stackPos,left);
        }
        Point up = {currentPoint.x,currentPoint.y + 1};
        if(isInRangeOrNotFlagged(img,flag,up))
        {
            addToStack(stack,&stackPos,up);
        }
        Point down = {currentPoint.x,currentPoint.y - 1};
        if(isInRangeOrNotFlagged(img,flag,down))
        {
            addToStack(stack,&stackPos,down);
        }
        
        if(P_Distance(&currentPoint,&upLeft) < P_Distance(&upLeft,&minUpLeft))
        {
            printf("upleft \n");
            minUpLeft = currentPoint;
        }
        if(P_Distance(&currentPoint,&upRight) < P_Distance(&upRight,&minUpRight))
        {
            minUpRight = currentPoint;
        }
        if(P_Distance(&currentPoint,&downLeft) < P_Distance(&downLeft,&minDownLeft))
        {
            minDownLeft = currentPoint;
        }
        if(P_Distance(&currentPoint,&downRight) < P_Distance(&downRight,&minDownRight))
        {
            minDownRight = currentPoint;
        }

        flag->data[currentPoint.x + currentPoint.y * flag->cols] = 1;

    }
    free(stack);

    Square square;
    square.points[0] = minUpLeft;
    square.points[1] = minUpRight;
    square.points[2] = minDownRight;
    square.points[3] = minDownLeft;
    return square;
}


Square GetSquareWithContour(Matrix* img)
{
    Matrix* flag = M_Create_2D(img->effectiveRows,img->effectiveCols);

    for (size_t i = 0; i < img->rows; i++)
    {
        for (size_t j = 0; j < img->cols; j++)
        {
            if(M_Get(img,i,j) == 0 || flag->data[j + i * flag->cols] >= 0.9)
            {
                continue;
            }
            Square square = GetOneSquare(img,flag,(Point){i,j});
            printf("before printing the square \n");
            S_Print(&square);
            printf("after \n");
            if(S_IsSquare(&square,0.1f))
            {
                printf("square found \n");
                M_Free(flag);
                return square;
            }
        }
    }
    printf("no square found \n");
    Square square;
    return square;
}
