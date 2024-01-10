#pragma once
#include "matrix/Matrix.h"

/*

Transformations on the image

*/


//Rotate the image by the angle
void M_RotateImage_1D(Matrix* image, Matrix* output, float angle, size_t width);

//Create the output matrix and rotate
Matrix* M_RotateImage_1DI(Matrix* image,float angle, size_t width);

//Zoom in or out in the image by the ratio, 1.0f = no zoom, >1.0f = zoom in, <1.0f = zoom out
void M_Zoom(Matrix* image, Matrix* output, float ratio);
//Create the output matrix and zoom
Matrix* M_ZoomI(Matrix* image,size_t width, float ratio);

//Smooth the image
void M_BillinearInterpolation(Matrix* image, Matrix* output);
//Create the output matrix and smooth
Matrix* M_IBillinearInterpolation(Matrix* image);

//Get the minimum value around the pixel for each pixel
void M_MinTransformation(Matrix* input, Matrix* output, size_t width);
//Create the output matrix and get the minimum value around the pixel for each pixel
Matrix* M_IMinTransformation(Matrix* input, size_t width);

//Change the resolution of the image by the ratio, 1.0f = no change, >1.0f = increase, <1.0f = decrease
Matrix* DownScale(const Matrix* input, float ratio);

//Change the resolution of image, the width or the height
Matrix* M_Stretch(const Matrix* input, size_t width, size_t height);

//Blur the image
Matrix* M_Blur(const Matrix* input);





