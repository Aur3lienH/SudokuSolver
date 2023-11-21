#include "Canny.h"
#include "../deepLearning/Matrix.h"
#include "../geometry/Square.h"
#include "../geometry/Point.h"
#include <math.h>

#define M_PI 3.14159265358979323846
const float thresold = 0.12f;

Matrix* canny(Matrix* input, float sigma)
{
    Matrix* kernel = M_Create_2D(5,5);

    float factor = 1.0f / (2.0f * M_PI * sigma * sigma);
    for (size_t i = 0; i < kernel->rows; i++)
    {
        for (size_t j = 0; j < kernel->cols; j++)
        {
            int distCenterX = abs((int)i - 2);
            int distCenterY = abs((int)j - 2);
            kernel->data[i * kernel->cols + j] = factor * exp(-(distCenterX * distCenterX + distCenterY * distCenterY) / (2.0f * sigma * sigma));
        }
    }
    float sum = M_GetSum(kernel);
    M_ScalarMul(kernel, 1.0f / sum, kernel);


    //Perform the convolution with valid padding
    Matrix* blurred = M_Create_2D(input->rows - kernel->rows + 1, input->cols - kernel->cols + 1);

    //Need to be implemented
    M_Convolution(input, kernel, blurred);
    M_Dim(blurred);
    M_Dim(kernel);
    M_Dim(input);



    Matrix* gX = M_Create_2D(1,3);
    gX->data[0] = -1;
    gX->data[1] = 0;
    gX->data[2] = 1;

    Matrix* gY = M_Create_2D(3,1);
    gY->data[0] = -1;
    gY->data[1] = 0;
    gY->data[2] = 1;

    Matrix* gradientX = M_Create_2D(blurred->rows, blurred->cols - gX->cols + 1);
    Matrix* gradientY = M_Create_2D(blurred->rows - 2, blurred->cols);


    M_Convolution(blurred, gX, gradientX);
    M_Convolution(blurred, gY, gradientY);


    Matrix* gradient = M_Create_2D(blurred->rows - 2, blurred->cols - 2);

    for (size_t i = 0; i < gradient->rows; i++)
    {
        for (size_t j = 0; j < gradient->cols; j++)
        {
            float gx = gradientX->data[i * gradientX->cols + j];
            float gy = gradientY->data[i * gradientY->cols + j];
            float value = sqrt(gx * gx + gy * gy);
            gradient->data[i * gradient->cols + j] = value;
        }
    }

    Matrix* nonMaxSuppression = M_Create_2D(gradient->rows, gradient->cols);
    for (size_t i = 0; i < nonMaxSuppression->rows; i++)
    {
        for (size_t j = 0; j < nonMaxSuppression->cols; j++)
        {
            float value = gradient->data[i * gradient->cols + j];
            if(value == 0)
            {
                nonMaxSuppression->data[i * nonMaxSuppression->cols + j] = 0;
            }
            else
            {
                float angle = atan2(gradientY->data[i * gradientY->cols + j], gradientX->data[i * gradientX->cols + j]);
                if(angle < 0)
                {
                    angle += M_PI;
                }
                if(angle >= 0 && angle < M_PI / 8.0f)
                {
                    float value1 = gradient->data[i * gradient->cols + j + 1];
                    float value2 = gradient->data[i * gradient->cols + j - 1];
                    if(value > value1 && value > value2)
                    {
                        nonMaxSuppression->data[i * nonMaxSuppression->cols + j] = value;
                    }
                    else
                    {
                        nonMaxSuppression->data[i * nonMaxSuppression->cols + j] = 0;
                    }
                }
                else if(angle >= M_PI / 8.0f && angle < 3.0f * M_PI / 8.0f)
                {
                    float value1 = gradient->data[(i + 1) * gradient->cols + j + 1];
                    float value2 = gradient->data[(i - 1) * gradient->cols + j - 1];
                    if(value > value1 && value > value2)
                    {
                        nonMaxSuppression->data[i * nonMaxSuppression->cols + j] = value;
                    }
                    else
                    {
                        nonMaxSuppression->data[i * nonMaxSuppression->cols + j] = 0;
                    }
                }
                else if(angle >= 3.0f * M_PI / 8.0f && angle < 5.0f * M_PI / 8.0f)
                {
                    float value1 = gradient->data[(i + 1) * gradient->cols + j];
                    float value2 = gradient->data[(i - 1) * gradient->cols + j];
                    if(value > value1 && value > value2)
                    {
                        nonMaxSuppression->data[i * nonMaxSuppression->cols + j] = value;
                    }
                    else
                    {
                        nonMaxSuppression->data[i * nonMaxSuppression->cols + j] = 0;
                    }
                }
                else if(angle >= 5.0f * M_PI / 8.0f && angle < 7.0f * M_PI / 8.0f)
                {
                    float value1 = gradient->data[(i + 1) * gradient->cols + j - 1];
                    float value2 = gradient->data[(i - 1) * gradient->cols + j + 1];
                    if(value > value1 && value > value2)
                    {
                        nonMaxSuppression->data[i * nonMaxSuppression->cols + j] = value;
                    }
                    else
                    {
                        nonMaxSuppression->data[i * nonMaxSuppression->cols + j] = 0;
                    }
                }
                else if(angle >= 7.0f * M_PI / 8.0f && angle < M_PI)
                {
                    float value1 = gradient->data[i * gradient->cols + j + 1];
                    float value2 = gradient->data[i * gradient->cols + j - 1];
                    if(value > value1 && value > value2)
                    {
                        nonMaxSuppression->data[i * nonMaxSuppression->cols + j] = value;
                    }
                    else
                    {
                        nonMaxSuppression->data[i * nonMaxSuppression->cols + j] = 0;
                    }
                }
            }
        }
    }



    return nonMaxSuppression;
}

float sumOfDistance(Point* p, Point* other, int nbPoints)
{
    float sum = 0;
    for (size_t i = 0; i < nbPoints; i++)
    {
        sum += P_Distance(p, &other[i]);
    }
    if(nbPoints == 0)
    {
        Point initialPoint = {0,0};
        return P_Distance(p, &initialPoint);
    }
    return sum;
}


Square* contourToSquare(Matrix* contour)
{
    Square* square = malloc(sizeof(Square));
    Point* finalPoints = (Point*)malloc(sizeof(Point) * 4);
    int nbPoints = 0;
    Point maxPoint = {0,0};


    for (size_t i = 0; i < 4; i++)
    {
        float maxDist = 0;
        for (size_t i = 0; i < contour->rows; i++)
        {
            for (size_t j = 0; j < contour->cols; j++)
            {
                
                if(contour->data[i * contour->cols + j] >= 0.9)
                {
                    Point p = {i, j};
                    float dist = sumOfDistance(&p, finalPoints, nbPoints);
                    if(dist > maxDist)
                    {
                        maxDist = dist;
                        maxPoint.x = i;
                        maxPoint.y = j;
                    }
                }
            }
        }
        finalPoints[nbPoints] = maxPoint;
        nbPoints++;
    }
    square->points[0] = finalPoints[0];
    square->points[1] = finalPoints[2];
    square->points[2] = finalPoints[1];
    square->points[3] = finalPoints[3];


    free(finalPoints);
    return square;
}

void RotateOrientation(Point* p)
{
    if(p->x == 1 && p->y > -1)
    {
        p->y -= 1;
    }
    else if(p->x == -1 && p->y < 1)
    {
        p->y += 1;
    }
    else if(p->y == 1 && p->x < 1)
    {
        p->x += 1;
    }
    else if(p->y == -1 && p->x > -1)
    {
        p->x -= 1;
    }
    
}

void fill(Matrix* matrix, Point p)
{
    for (size_t i = 0; i < matrix->rows; i++)
    {
        int j = 0;
        int jPrevious = -1;
        while(j < matrix->cols)
        {
            if(matrix->data[j + i * matrix->cols] >= 0.9)
            {
                if(jPrevious == -1)
                {
                    jPrevious = j;
                }
                else
                {
                    for (int k = jPrevious; k < j; k++)
                    {
                        matrix->data[j + i * matrix->cols] = 1.0f;
                    }
                    jPrevious = -1;
                }

            }
            j++;
        }
    }
    
}

Point contourDetection(Matrix* canny,Matrix* output,Matrix* flag, Point actual,Point initialPoint, Point direction)
{
    size_t loopCounter = 0;
    int isInitialPointVisited = 0;
    Point emptyPoint = {-1,-1};
    while (!isInitialPointVisited || loopCounter == 0) {
        for (size_t i = 0; i < 7; i++) {
            RotateOrientation(&direction);
            Point next = P_Add(&actual, &direction);
            
            if (next.x >= 0 && next.x < canny->rows && next.y >= 0 && next.y < canny->cols) {
                if(flag->data[next.x * output->cols + next.y] >= 0.4)
                {
                    return;
                }
                if (canny->data[next.x * canny->cols + next.y] >= 0.4) {
                    output->data[actual.x * output->cols + actual.y] = 1.0;
                    //printf("actual : %d, %d\n",actual.x,actual.y);
                    flag->data[actual.x * output->cols + actual.y] = 1.0;

                    // Reverse the direction
                    direction.x = -direction.x;
                    direction.y = -direction.y;
                    
                    {
                        Point temp = {-direction.x,-direction.y};
                        RotateOrientation(&temp);
                        if(!P_Equals(&next,&temp) && temp.x >= 0 && temp.x < canny->rows && temp.y >= 0 && temp.y < canny->cols && flag->data[temp.x * canny->cols + temp.y] < 0.1f)
                        {
                            emptyPoint = temp;
                        }
                    }
                    actual = next;
                    break;
                }
            }
        }

        // Check if the initial point is revisited
        if (P_Equals(&actual, &initialPoint)) {
            if (loopCounter > 0) {
                isInitialPointVisited = 1;
            }
        }

        loopCounter++;
    }
    return emptyPoint;
}

void basicContourDetection(Matrix* input, Matrix* output, Point p)
{

}

// Get the square of the sudoku
Matrix* getSquare(Matrix* canny)
{
    return canny;
    Matrix* contour = M_Create_2D(canny->rows, canny->cols);
    Matrix* squareMatrix = M_Create_2D(canny->rows, canny->cols);
    Matrix* flag = M_Create_2D(canny->rows, canny->cols);
    size_t squareCount = 0;
    for (size_t i = 0; i < canny->rows; i++)
    {
        for (size_t j = 0; j < canny->cols; j++)
        {
            if(canny->data[i * canny->cols + j] >= 0.8 && flag->data[i * contour->cols + j] < 0.1)
            {
                printf("here\n");
                //Point emptyPoint = contourDetection(canny, contour,flag, (Point){i,j}, (Point){i,j}, (Point){0,1});
                //fill(flag,emptyPoint);
                //fill(canny,emptyPoint);
                //Square* square = contourToSquare(contour);
                //printf("Square found \n");
                //S_Draw(squareMatrix, square, 1.0);
                //S_Draw(squareMatrix, square, 1.0);
                squareCount++;
                if(squareCount == 100)
                    return flag;
            }
        }
    }
    return contour;
}