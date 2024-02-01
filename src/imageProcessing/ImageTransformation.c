#include "imageProcessing/ImageTransformation.h"
#include <math.h>


void rotatePoint(float* pointX, float* pointY, const float centerX, const float centerY, double angleDegree) {
    // Convert angle to radians
    float angleRad = angleDegree * (3.14 / 180.0);

    // Translate the point to the origin (relative to the center)
    float tempX = *pointX - centerX;
    float tempY = *pointY - centerY;

    // Perform the rotation
    float rotatedX = tempX * cos(angleRad) - tempY * sin(angleRad);
    float rotatedY = tempX * sin(angleRad) + tempY * cos(angleRad);

    *pointX = rotatedX + centerX;
    *pointY = rotatedY + centerY;
}

float GetSurrondingAverage(Matrix* image, size_t x, size_t y, size_t width, size_t height) {
    float sum = 0;
    size_t count = 0;
    for (int i = -1; i < 2; i++) {
        if (x + i < 0 || x + i >= width) {
            continue;
        }
        for (int j = -1; j < 2; j++) {
            if (y + j < 0 || y + j >= height) {
                continue;
            }
            sum += image->data[(int)(x + i) + (int)(y + j) * width];
            count++;
        }
    }
    return sum / count;
}

float GetSurrondingAverage3D(Matrix* image, size_t x, size_t y, size_t width, size_t height)
{
    float sum = 0;
    size_t count = 0;
    for (int i = -1; i < 2; i++) {
        size_t xIndex = x + i;
        if (xIndex < 0 || xIndex >= width) {
            continue;
        }
        for (int j = -1; j < 2; j++) {
            size_t yIndex = y + j;
            if (yIndex < 0 || yIndex >= height) {
                continue;
            }
            sum += image->data[((int)(xIndex) + (int)(yIndex) * width) * 3];
            count++;
        }
    }
    return sum / count;
}

float GetSurrondingMinimum(Matrix* image, size_t x, size_t y, size_t width)
{
    float min = 1;
    for (int i = -1; i < 2; i++) {
        if (x + i < 0 || x + i >= width) {
            continue;
        }
        for (int j = -1; j < 2; j++) {
            if (y + j < 0 || y + j >= width) {
                continue;
            }
            if (image->data[(int)(x + i) + (int)(y + j) * width] < min)
            {
                min = image->data[(int)(x + i) + (int)(y + j) * width];
            }
        }
    }
    return min;
}

float linear_interpolation(float value1, float value2, float distance)
{
    return value1 * (1 - distance) + value2 * distance;
}

float getValue(int x, int y, size_t width, Matrix* image)
{
    int isCorrect = x >= 0 && x < width && y >= 0 && y < width;
    if(!isCorrect)
    {
        return 0;
    }
    return image->data[x + y * width];
}

float billinear_interpolation(float x, float y, Matrix* image, size_t width)
{
    int x1 = floor(x);
    int x2 = ceil(x);
    int y1 = floor(y);
    int y2 = ceil(y);

    float Q11 = getValue(x1, y1, width, image);
    float Q12 = getValue(x1, y2, width, image);
    float Q21 = getValue(x2, y1, width, image);
    float Q22 = getValue(x2, y2, width, image);

    float R1 = linear_interpolation(Q11, Q21, x - x1);
    float R2 = linear_interpolation(Q12, Q22, y - y1);
    return linear_interpolation(R1, R2,y - y1);
}


void M_RotateImage_1D(Matrix* image, Matrix* output, float angle, size_t width) {
    size_t xOrigin = width / 2;
    size_t yOrigin = width / 2;

    float cosAngle = cosf(angle);
    float sinAngle = sinf(angle);

    // Iterate over every pixel of the output image
    for (size_t i = 0; i < width; i++) {
        for (size_t j = 0; j < width; j++) {

            float pointX = i;
            float pointY = j;
            
            
            rotatePoint(&pointX, &pointY, xOrigin, yOrigin, angle);

            
            // Set the pixel value
            size_t index = (int)pointX + (int)pointY * width;
            if (index < 0 || index >= width * width) {
                continue;
            }
            output->data[i + j * width] = billinear_interpolation(pointX, pointY, image, width);

            
        }
    }

    
}

Matrix* M_RotateImage_1DI(Matrix* image,float angle, size_t width) {
    Matrix* output = M_Create_2D(width* width,1);
    M_RotateImage_1D(image,output,angle,width);
    return output;
}

void M_Zoom(Matrix* image, Matrix* output,float ratio)
{
    size_t width = image->rows; // assuming the image is square, so the width = sqrt(rows)
    
    float halfWidth = width / 2.0f;  // center of the image

    // iterate through the output matrix
    for (size_t i = 0; i < width; i++) {
        for (size_t j = 0; j < width; j++) {
            
            // calculate the difference from the center for both x and y
            float diffX = (i - halfWidth) * ratio;
            float diffY = (j - halfWidth) * ratio;

            // calculate the original image coordinates based on the difference and center
            float originalX = halfWidth + diffX;
            float originalY = halfWidth + diffY;

            // get interpolated value from the original image
            float interpolatedValue = billinear_interpolation(originalX, originalY, image, width);
            
            // set the interpolated value to the output matrix
            output->data[i + j * width] = interpolatedValue;
        }
    }
    
}

Matrix* M_ZoomI(Matrix* image,size_t width, float ratio)
{
    Matrix* output = M_Create_2D(width, width);
    M_Zoom(image, output, ratio);
    return output;
}

void M_BillinearInterpolation(Matrix* image, Matrix* output)
{
    for (size_t i = 0; i < 28; i++)
    {
        for (size_t j = 0; j < 28; j++)
        {
            output->data[i * 28 + j] = billinear_interpolation(j, i,image, 28);
        }
    }
}

Matrix* M_IBillinearInterpolation(Matrix* image)
{
    Matrix* res = M_Create_2D(28 * 28, 1);
    M_BillinearInterpolation(image, res);
    return res;
}



void M_MinTransformation(Matrix* input, Matrix* output, size_t width)
{
    for (size_t i = 0; i < width; i++)
    {
        for (size_t j = 0; j < width; j++)
        {
            output->data[i * width + j] = GetSurrondingMinimum(input, j, i, width);
        }
    }
}

Matrix* M_IMinTransformation(Matrix* input, size_t width)
{
    Matrix* res = M_Create_2D(width * width, 1);
    M_MinTransformation(input, res, width);
    return res;
}

//Reduce the resolution of the image by a ratio
Matrix* DownScale(const Matrix* input, float ratio)
{
    size_t newWidth = (size_t)(input->cols * ratio);
    size_t newHeight = (size_t)(input->rows * ratio);
    Matrix* res = M_Create_2D(newHeight, newWidth);

    for (size_t i = 0; i < newHeight; i++)
    {
        for (size_t j = 0; j < newWidth; j++)
        {
            res->data[i * newWidth + j] = GetSurrondingAverage((Matrix *)input, j / ratio, i / ratio, input->cols, input->rows);
        }
    }
    return res;
}

Matrix* DownScale3D(const Matrix* input, float ratio)
{
    size_t newWidth = (size_t)(input->cols * ratio);
    size_t newHeight = (size_t)(input->rows * ratio);
    Matrix* res = M_Create_3D(newHeight, newWidth, input->dims);
    for (size_t i = 0; i < newHeight; i++)
    {
        for (size_t j = 0; j < newWidth; j++)
        {
            for (size_t k = 0; k < input->dims; k++)
            {
                res->data[(i * newWidth + j) * 3 + k] = input->data[(int)(i / ratio) * input->cols * 3 + (int)(j / ratio) * 3 + k];
            }
        }
    }
    return res;
}

Matrix* M_Stretch(const Matrix* input, size_t width, size_t height)
{
    Matrix* res = M_Create_2D(width, height);
    float x_ratio = (float)input->cols / (float)width;
    float y_ratio = (float)input->rows / (float)height;
    size_t px, py;

    for (size_t i = 0; i < height; i++)
    {
        for (size_t j = 0; j < width; j++)
        {   
            px = floor(j * x_ratio);
            py = floor(i * y_ratio);
            res->data[j + i * width] = input->data[px + py * input->cols];
        }
    }
    return res;
}


Matrix* M_Blur(const Matrix* input)
{
    Matrix* res = M_Create_2D(input->rows, input->cols);
    for (size_t i = 0; i < input->rows; i++)
    {
        for (size_t j = 0; j < input->cols; j++)
        {
            res->data[i * input->cols + j] = GetSurrondingAverage((Matrix *)input, j, i, input->cols, input->rows);
        }
    }
    return res;
}


Matrix* M_GaussianBlur(const Matrix* input, float sigma)
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
    return blurred;
}