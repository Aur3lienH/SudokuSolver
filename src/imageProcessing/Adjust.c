#include "imageProcessing/Adjust.h"
#include "geometry/Point.h"


void AdjustLine(Matrix* img,Point* prevLeft, Point* newLeft, Point* prevRigth, Point* newRight)
{
    Matrix* imgCopy = M_CopyI(img);
    printf("prevLeft : %i %i\n",prevLeft->x,prevLeft->y);
    printf("prevRigth : %i %i\n",prevRigth->x,prevRigth->y);
    printf("newLeft : %i %i\n",newLeft->x,newLeft->y);
    printf("newRight : %i %i\n",newRight->x,newRight->y);
    float prevDistance = P_Distance(prevLeft, prevRigth);
    float newDistance = P_Distance(newLeft, newRight);



    printf("prevDistance : %f\n",prevDistance);
    printf("newDistance : %f\n",newDistance);
    float ratio = newDistance / prevDistance;
    //Get the change need to be done 
    size_t y = prevRigth->y;
    for(size_t i = 0; i < imgCopy->cols; i++)
    {
        if(i < newLeft->x)
        {
            P_DrawPixel(imgCopy, i, y, Color_Create(0,0,0));
        }
        else if(i > newRight->x)
        {
            P_DrawPixel(imgCopy, i, y, Color_Create(0,0,0));
        }
        else
        {
            float x = (i - newLeft->x) * ratio + prevLeft->x;

            P_DrawPixel(imgCopy, x, y, P_GetColor(img, i ,y));
            //P_DrawPixelFrom3D(imgCopy, i, y, (int)x, y);
        }
    }
    M_Copy(imgCopy, img);
}

int ContainsPoint(Point* p, PointSet* pointSet)
{
    for (size_t i = 0; i < pointSet->size; i++)
    {
        if (P_Equals(p, &pointSet->points[i]))
        {
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


void ExReImages(Matrix* img, PointSet* squarePointSet, PointSet* newPointSet)
{

    

    for (size_t i = 0; i < img->rows; i++)
    {
        Point* points = malloc(sizeof(Point) * 4);
        points[0] = points[1] = points[2] = points[3] = (Point){-1,-1}; // Initialize all points to (-1, -1)


        
            
        for (size_t k = 0; k < squarePointSet->size; k++)
        {
            if (squarePointSet->points[k].y == i)
            {
                // Check for leftmost point
                if (points[0].x == -1 || squarePointSet->points[k].x < points[0].x)
                {
                    points[0] = squarePointSet->points[k];
                }
                // Check for rightmost point
                if (points[1].x == -1 || squarePointSet->points[k].x > points[1].x)
                {
                    points[1] = squarePointSet->points[k];
                }
            }
        }

        for (size_t k = 0; k < newPointSet->size; k++)
        {
            if (newPointSet->points[k].y == i)
            {
                // Check for leftmost point
                if (points[2].x == -1 || newPointSet->points[k].x < points[2].x)
                {
                    points[2] = newPointSet->points[k];
                }
                // Check for rightmost point
                if (points[3].x == -1 || newPointSet->points[k].x > points[3].x)
                {
                    points[3] = newPointSet->points[k];
                }
            }
        }

        int found = 0;
        for (size_t k = 0; k < 4; k++)
        {
            if(points[k].x != -1)
            {
                found++;
            }
        }

        if(points[0].x != -1 && points[1].x != -1 && points[2].x != -1 && points[3].x != -1)
        {
            if(points[0].x != points[1].x && points[2].x != points[3].x)
            {
                //printf("Adjusting line\n");
                AdjustLine(img, &points[0], &points[2], &points[1], &points[3]);
            }
            //printf("Adjusting line\n");
            //AdjustLine(img, &points[0], &points[2], &points[1], &points[3]);
        }

    }

    
}


void M_AdjustBorder(Matrix* img, const SquareDetectionResult sdr)
{
    Square square = sdr.square;
    PointSet* pointSet = sdr.pointSet;

    //Draw all the points of the square
    P_PointSetDraw(img, pointSet, Color_Create(0,255,0),0);

    //Initialize the pointSet in which the points line will be
    PointSet* linesPointSet = malloc(sizeof(PointSet));
    linesPointSet->points = malloc(sizeof(Point) * 10);
    linesPointSet->size = 0;
    //This is the size of the pointSet in which the points line will be
    size_t linesPointSetSize = 10;
    for (size_t i = 0; i < 4; i++)
    {
        //Get the 2 points of the line and the direction of the line
        Point direction;
        size_t j = (i + 1) % 4;
        //Point one of the line 
        Point p = square.points[i];
        //Point two of the line 
        Point p2 = square.points[j];

        PointSet* poinBetween = P_GetAllPointBetween(img, &p, &p2);
        linesPointSet->points = realloc(linesPointSet->points, sizeof(Point) * (linesPointSet->size + poinBetween->size));
        for (size_t k = 0; k < poinBetween->size; k++)
        {
            linesPointSet->points[linesPointSet->size] = poinBetween->points[k];
            linesPointSet->size++;
        }

    }
    

    ExReImages(img, pointSet, linesPointSet);
}