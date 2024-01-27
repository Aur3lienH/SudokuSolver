#include "imageProcessing/Grayscale.h"
#include "imageProcessing/Canny.h"
#include "imageProcessing/Hough.h"
#include <time.h>
#include "geometry/Square.h"
#include "geometry/Point.h"
#include "imageProcessing/DigitProcess.h"
#include "imageProcessing/ImageTransformation.h"
#include "imageProcessing/Image.h"
#include "matrix/Matrix.h"
#include <math.h>


Matrix* resize(const Matrix* input, size_t downSizeWidth)
{
	size_t maximum = input->cols > input->rows ? input->cols : input->rows;
	float ratio = (float)downSizeWidth / (float)maximum;
	return DownScale(input,ratio);
}

Matrix* PreprocessToCanny(Image* input, size_t downSizeWidth)
{
	Matrix* grayscaled = GrayscaleToMatrix(input);
	Matrix* resized = resize(grayscaled, downSizeWidth);
	Matrix* canny = Canny(resized, 1);
	M_Free(grayscaled);
	M_Free(resized);
	return canny;
}




Matrix* PreprocessSquare(Matrix* input)
{
	
}


Matrix* GaussianBlur(Matrix* input,float sigma)
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


	M_Convolution_ZeroPad(input, kernel, blurred);
	return blurred;
}

void StrechSudoku(Matrix* input, float* shape)
{
    
}