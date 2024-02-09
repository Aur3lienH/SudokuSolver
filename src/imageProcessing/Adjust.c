#include "imageProcessing/Adjust.h"



int AdjustLine(Matrix* img,Point* prevLeft, Point* newLeft, Point* prevRigth, Point* newRight)
{
    //Calculate the distance between the two points
    int distance = P_Distance(prevLeft, prevRigth);
    //Calculate the distance between the two new points
    int newDistance = P_Distance(newLeft, newRight);
    //Calculate the ratio between the two distances
    float ratio = (float)newDistance / (float)distance;
    //Change the pixels in between the two new points
    
    Point startPoint = *newLeft;
    Point endPoint = *newRight;
    float xDiff = endPoint.x - startPoint.x;
    float yDiff = endPoint.y - startPoint.y;
    int xDiffAbs = abs(xDiff);
    int yDiffAbs = abs(yDiff);
    int directionX = xDiff > yDiff ? 1 : 0;
    int directionY = yDiff > xDiff ? 1 : 0;
    
    while(!P_Equals(&startPoint, &endPoint) && (startPoint.x < img->cols && startPoint.y < img->rows) && (startPoint.x >= 0 && startPoint.y >= 0))
    {
        int x = startPoint.x * ratio;
        int y = startPoint.y * ratio;
        P_DrawPixelFrom3D(img, startPoint.x, startPoint.y, x, y);
        if(directionX)
        {
            startPoint.x += 1;
        }
        else
        {
            startPoint.y += 1;
        }
    }

}

int ContainsPoint(Point* p, PointSet* pointSet)
{
    for (size_t i = 0; i < pointSet->size; i++)
    {
        if (P_Equals(p, &pointSet->points[i]))
        {
            printf("i : %i\n",i);
            return 1;
        }
    }
    return 0;
}

Point GetPointFromSet(Point* intialPoint, PointSet* pointSet, Point* direction)
{
    Point p = *intialPoint;
    int xAdd = direction->x;
    int yAdd = direction->y;

    if(ContainsPoint(&p, pointSet))
    {
        return p;
    }
    for (size_t i = 0; i < 500; i++)
    {
        int xAddTemp = xAdd*(i+1);
        int yAddTemp = yAdd*(i+1);

        p.x = intialPoint->x + xAddTemp;
        p.y = intialPoint->y + yAddTemp;

        if(ContainsPoint(&p, pointSet))
        {
            return p;
        }

        p.x = intialPoint->x - xAddTemp;
        p.y = intialPoint->y - yAddTemp;

        if(ContainsPoint(&p, pointSet))
        {
            return p;
        }
    }
    
    //printf("return in the outside of the loop \n");
    return p;
    
}


Point GetCorrespondingPoint(size_t x, PointSet pointSet)
{
    for (size_t i = 0; i < pointSet.size; i++)
    {
        if(pointSet.points[i].x == x)
        {
            return pointSet.points[i];
        }
    }
    return (Point){0,0};
}

void M_AdjustBorder(Matrix* img, SquareDetectionResult sdr)
{
    Square square = sdr.square;
    PointSet* pointSet = sdr.pointSet;
    printf("PointSet size : %i\n",pointSet->size);
    P_PointSetDraw(img, pointSet, Color_Create(0,255,0),0);
    PointSet* linesPointSet = malloc(sizeof(PointSet) * 4);
    for (size_t i = 0; i < 4; i++)
    {
        size_t linesPointSetSize = 1;
        linesPointSet[i].points = malloc(sizeof(Point) * linesPointSetSize);
        linesPointSet[i].size = 0;
        printf("i : %i\n",i);
        Point direction;
        size_t j = (i + 1) % 4;
        Point p = square.points[i];
        Point p2 = square.points[j];
        PointSet* linePointSet = P_GetAllPointBetween(p, p2, &direction);
        P_GetPerpendicular(&direction);
        for (size_t j = 0; j < linePointSet->size; j++)
        {
            printf("j : %i\n",j);
            if(linesPointSet[i].size + 2 >= linesPointSetSize)
            {
                printf("realloc\n");
                linesPointSetSize *= 2;
                linesPointSet[i].points = realloc(linesPointSet[i].points, sizeof(Point) * linesPointSetSize);
            }
            linesPointSet[i].points[j*2] = linePointSet->points[i];
            linesPointSet[i].points[j*2+1] = GetPointFromSet(&p, pointSet, &direction);
            linesPointSet[i].size += 2;
        }
    }
    printf("heyhey\n");
    for (size_t i = 0; i < linesPointSet[1].size; i++)
    {
        Point firstPoint = linesPointSet[1].points[i];
        Point p = GetCorrespondingPoint(firstPoint.x, linesPointSet[3]);
        printf("firstPoint : %i,%i\n",firstPoint.x,firstPoint.y);
        printf("p : %i,%i\n",p.x,p.y);  
        P_DrawSegment(img, &firstPoint, &p, Color_Create(255,0,0),0);
    }
    
}