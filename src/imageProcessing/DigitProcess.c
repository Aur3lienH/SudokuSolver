#include "imageProcessing/DigitProcess.h"
#include "matrix/Matrix.h"
#include "imageProcessing/ImageProcessing.h"
#include "imageProcessing/ImageTransformation.h"
#include "imageProcessing/Image.h"
#include "geometry/Point.h"
#include <math.h>
//const size_t REMOVE_CORNER_WIDTH = 4;
const float REMOVE_CORNER_RATIO = 0.05f;




void RemoveCorners(Matrix* input, size_t width)
{
    size_t REMOVE_CORNER_WIDTH = (size_t)(REMOVE_CORNER_RATIO * (float)width);
    for (size_t i = 0; i < REMOVE_CORNER_WIDTH; i++)
    {
        for (size_t j = 0; j < width; j++)
        {
            input->data[i * width + j] = 0;
        }
    }
    for (size_t i = input->rows - REMOVE_CORNER_WIDTH; i < input->rows; i++)
    {
        for (size_t j = 0; j < width; j++)
        {
            input->data[i * width + j] = 0;
        }
    }
    for (size_t i = 0; i < input->rows; i++)
    {
        for (size_t j = 0; j < REMOVE_CORNER_WIDTH; j++)
        {
            input->data[i * width + j] = 0;
        }
    }
    for (size_t i = 0; i < input->rows; i++)
    {
        for (size_t j = input->cols - REMOVE_CORNER_WIDTH; j < input->cols; j++)
        {
            input->data[i * width + j] = 0;
        }
    }
}

Matrix* Downsize(const Matrix* input, size_t width, size_t height)
{
    Matrix* res = M_Create_2D(width,height);
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



Point* GetCenterPoint(const Matrix* input)
{
    float centerX = 0;
    float centerY = 0;

    for (size_t i = 0; i < input->rows; i++)
    {
        for (size_t j = 0; j < input->cols; j++)
        {   
            centerX += input->data[j + i * input->cols] * j;
            centerY += input->data[j + i * input->cols] * i;
        }
    }

    centerX /= M_GetSum(input);
    centerY /= M_GetSum(input);

    return P_Create(centerX,centerY);
}


Matrix* Center(const Matrix* input, Point* offset)
{
    Matrix* res = M_Create_2D(input->rows,input->cols);
    for (size_t i = 0; i < input->rows; i++)
    {
        for (size_t j = 0; j < input->cols; j++)
        {
            size_t x = j + offset->x;
            size_t y = i + offset->y;
            if(x < input->cols && y < input->rows)
            {
                res->data[x + y * input->cols] =  input->data[j + i * input->cols];
            }
        }
    }
    return res;
}


Point GetDigitCenter(Matrix* cell)
{
    //Take the center as starting point
    Point* startingPoint = P_Create(cell->cols / 2, cell->rows / 2);
    if(M_GetPoint(cell,startingPoint))
    {
        return *startingPoint;
    }

    //If the center is not black, search for the first black pixel around
    //WARNING : This is not the most efficient way to do it 
    //The best way to do it would be to not check the same point at the center each time
    int distance = 1;
    while(1)
    {
        for (int i = -distance; i <= distance; i++)
        {
            for (int j = -distance; j <= distance; j++)
            {
                if(i == 0 && j == 0) continue;
                Point* p = P_Create(startingPoint->x + i, startingPoint->y + j);
                if(M_GetPoint(cell,p))
                {
                    return *p;
                }
                
                P_Free(p);
            }
        }
        distance++;
    }
    return *startingPoint;
}

int IsBlank(const Matrix* m)
{
    float ratio = 0.2;
    size_t distance = m->rows * ratio;
    size_t width = m->rows - distance * 2;
    float sum = 0;
    for (size_t i = distance; i < m->rows - distance; i++)
    {
        for (size_t j = distance; j < m->cols - distance; j++)
        {
            sum += m->data[j + i * m->cols];
        }
    }
    return (sum/((width * width))) < 0.1f;
    
}

Matrix* SurfaceToDigit(const Image* image, int* isBlankPtr)
{
    Matrix* res = ImageToMatrix(image);
    return MatrixToDigit(res,isBlankPtr);
}

Matrix* MatrixToDigit(Matrix* matrix, int* isBlankPtr)
{
    /*
    M_Dim(downSized);
    RemoveCorners(downSized,28);
    Matrix* centered = CenterDigit(downSized,28);
    Matrix* zoomed = M_ZoomI(centered,28,0.6);
    Matrix* output = M_IBillinearInterpolation(zoomed);
    for (size_t i = 0; i < 0; i++)
    {
        output = M_IBillinearInterpolation(output);
    }
    Matrix* downSized = Downsize(res,28,28);
    output = CenterDigit(output,28);
    */
   RemoveCorners(matrix,matrix->rows);
   Matrix* input = M_ZoomI(matrix,matrix->rows,0.75);
   if(isBlankPtr != NULL) *isBlankPtr = IsBlank(matrix);
   Matrix* centered = CenterDigit(input,input->rows);
   Matrix* downSized = Downsize(centered,28,28);
    return downSized;
}













