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

    Matrix* thresholded = M_Create_2D(nonMaxSuppression->rows, nonMaxSuppression->cols);
    for (size_t i = 0; i < nonMaxSuppression->rows; i++)
    {
        for (size_t j = 0; j < nonMaxSuppression->cols; j++)
        {
            float value = nonMaxSuppression->data[i * nonMaxSuppression->cols + j];
            if(value > thresold)
            {
                thresholded->data[i * thresholded->cols + j] = 1.0f;
            }
            else
            {
                thresholded->data[i * thresholded->cols + j] = 0.0f;
            }
        }
    }
    



    return thresholded;
}






