#include "imageProcessing/Canny.h"
#include "matrix/Matrix.h"
#include "geometry/Square.h"
#include "geometry/Point.h"
#include "imageProcessing/Grayscale.h"
#include <math.h>
#include <err.h>





float normalizeAngle(float angle)
{
    if(angle < 0)
    {
        angle += M_PI;
    }
    if(angle >= M_PI)
    {
        angle -= M_PI;
    }
    return angle;
}

void comparePixels(Matrix* input, Matrix* output, size_t initialPoint, size_t point1, size_t point2)
{
    if(input->data[initialPoint] < input->data[point1] || input->data[initialPoint] < input->data[point2])
    {
        output->data[initialPoint] = 0;
    }
    else
    {
        output->data[initialPoint] = input->data[initialPoint];
    }
}

int quantize_angle(float angle)
{
    angle = angle * 180.0f / M_PI;
    if(angle < 22.5)
    {
        return 0;
    }
    else if(angle < 67.5)
    {
        return 45;
    }
    else if(angle < 112.5)
    {
        return 90;
    }
    else if(angle < 157.5)
    {
        return 135;
    }
    else
    {
        return 0;
    }
}


void AddToStack(Point* stack, size_t* pos, size_t x, size_t y)
{
    stack[*pos] = (Point){.x = x, .y = y};
    *pos += 1;
}

void pointToOne(Matrix* matrix, Point* points, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        Point currentPoint = points[i];
        matrix->data[currentPoint.y * matrix->cols + currentPoint.x] = 1;
    }
}

void cleanUp(Matrix* m)
{
    for (size_t i = 0; i < m->rows; i++)
    {
        for (size_t j = 0; j < m->cols; j++)
        {
            if(m->data[i * m->cols + j] == 0.5f)
            {
                m->data[i * m->cols + j] = 0.0f;
            }
        }
        
    }
    
}

void propagate(Matrix* input, size_t posX, size_t posY)
{
    //Allocate a stack
    Point* stack = malloc(sizeof(Point) * input->cols * input->rows);
    //Allocate points to store all the points and set them to 1 at the end
    Point* AllPoints = malloc(sizeof(Point) * input->cols * input->rows);

    //Initialize the positions
    size_t allPointsPos = 0;
    size_t stackPos = 0;

    //Add the first point to the stack
    AddToStack(stack, &stackPos, posX, posY);

    AddToStack(AllPoints, &allPointsPos, posX, posY);
    
    //While the stack is not empty
    while(stackPos != 0)
    {
        //Pop the stack
        stackPos -= 1;
        Point currentPoint = stack[stackPos];
        
        //x and y are the coordinates of the current point
        size_t x = currentPoint.x;
        size_t y = currentPoint.y;
        //Check if the point is out of bounds
        if(x < 0 || x >= input->cols || y < 0 || y >= input->rows)
        {
            continue;
        }
        //Check if the point is already visited or if it is not an edge, if it is go to the next point
        if(input->data[y * input->cols + x] <= 0.1)
        {
            continue;
        }
        
        //Check if the point is a an strong edge, if it is, add it to the points and set it to 1
        if(input->data[y * input->cols + x] >= 0.9)
        {
            pointToOne(input, AllPoints, allPointsPos);
            break;
        }
        //Add the point to the points
        AllPoints[allPointsPos] = currentPoint;
        allPointsPos += 1;

        //Set the current point to 0
        input->data[y * input->cols + x] = 0;
        
        //Add the neighbors to the stack
        AddToStack(stack, &stackPos, x - 1, y - 1);
        AddToStack(stack, &stackPos, x - 1, y);
        AddToStack(stack, &stackPos, x - 1, y + 1);
        AddToStack(stack, &stackPos, x, y - 1);
        AddToStack(stack, &stackPos, x, y + 1);
        AddToStack(stack, &stackPos, x + 1, y - 1);
        AddToStack(stack, &stackPos, x + 1, y);
        AddToStack(stack, &stackPos, x + 1, y + 1);
    }
    free(stack);
    free(AllPoints);
    
}


Matrix* Canny(Matrix* input, float sigma)
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
    Matrix* blurred = M_Create_2D(input->rows, input->cols);

    //Need to be implemented
    M_Convolution_ZeroPad(input, kernel, blurred);
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

    Matrix* gradientX = M_Create_2D(blurred->rows, blurred->cols);
    Matrix* gradientY = M_Create_2D(blurred->rows, blurred->cols);


    M_Convolution_ZeroPad(blurred, gX, gradientX);
    M_Convolution_ZeroPad(blurred, gY, gradientY);


    Matrix* gradient = M_Create_2D(blurred->rows, blurred->cols);

    for (size_t i = 0; i < gradient->rows; i++)
    {
        for (size_t j = 0; j < gradient->cols; j++)
        {
            float gx = gradientX->data[i* gradientX->cols + j];
            float gy = gradientY->data[i * gradientY->cols + j];
            float value = sqrt(gx * gx + gy * gy);
            gradient->data[i * gradient->cols + j] = value;
        }
    }
    M_SaveImage(gradient, "images/export/preproc_1.jpg");
    

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
                float normalizedAngle = normalizeAngle(angle);
                size_t cols = nonMaxSuppression->cols;
                size_t rows = nonMaxSuppression->rows;
                switch (quantize_angle(normalizedAngle))
                {
                case 0:
                    if(j == 0 || j == cols - 1)
                    {
                        nonMaxSuppression->data[i * nonMaxSuppression->cols + j] = 0;
                        break;
                    }
                    comparePixels(gradient, nonMaxSuppression, i * cols + j, i * cols + j - 1, i * cols + j + 1);
                    break;
                    
                case 45:
                    if(i == 0 || i == rows - 1 || j == 0 || j == cols - 1)
                    {
                        nonMaxSuppression->data[i * nonMaxSuppression->cols + j] = 0;
                        break;
                    }
                    comparePixels(gradient, nonMaxSuppression, i * cols + j, (i - 1) * cols + j - 1, (i + 1) * cols + j + 1);
                    break;
                case 90:
                    if(i == 0 || i == rows - 1)
                    {
                        nonMaxSuppression->data[i * nonMaxSuppression->cols + j] = 0;
                        break;
                    }
                    comparePixels(gradient, nonMaxSuppression, i * cols + j, (i - 1) * cols + j, (i + 1) * cols + j);
                    break;
                case 135:
                    if(i == 0 || i == rows - 1 || j == 0 || j == cols - 1)
                    {
                        nonMaxSuppression->data[i * nonMaxSuppression->cols + j] = 0;
                        break;
                    }
                    comparePixels(gradient, nonMaxSuppression, i * cols + j, (i - 1) * cols + j + 1, (i + 1) * cols + j - 1);
                    break;
                default:
                    errx(-1, "Angle not found");
                    break;
                }

            }
        }
    }

    M_SaveImage(nonMaxSuppression, "images/export/preproc_2.jpg");

    float maxGradient = M_GetMax(nonMaxSuppression);
    float highThresold = 0.3 * maxGradient; 
    float lowThresold = 0.15 * maxGradient;


    Matrix* thresholded = M_Create_2D(nonMaxSuppression->rows, nonMaxSuppression->cols);
    for (size_t i = 0; i < nonMaxSuppression->rows; i++)
    {
        for (size_t j = 0; j < nonMaxSuppression->cols; j++)
        {
            float value = nonMaxSuppression->data[i * nonMaxSuppression->cols + j];
            if(value > highThresold)
            {
                thresholded->data[i * thresholded->cols + j] = 1.0f;
            }
            else if (value > lowThresold)
            {
                thresholded->data[i * thresholded->cols + j] = 0.5f;
            }
            else
            {
                thresholded->data[i * thresholded->cols + j] = 0.0f;
            }
        }
    }

    M_SaveImage(thresholded, "images/export/preproc_3.jpg");
    
    for (size_t i = 0; i < thresholded->rows; i++)
    {
        for (size_t j = 0; j < thresholded->cols; j++)
        {
            if(thresholded->data[i * thresholded->cols + j] >= 0.4 && thresholded->data[i * thresholded->cols + j] <= 0.6)
            {
                propagate(thresholded, j, i);
            }
        }
    }
    

    
    M_SaveImage(thresholded, "images/export/preproc_4.jpg");
    return thresholded;
}






