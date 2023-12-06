#pragma once
#include "Matrix.h"


void M_RotateImage_1D(Matrix* image, Matrix* output, float angle, size_t width);

Matrix* M_RotateImage_1DI(Matrix* image,float angle, size_t width);

void M_Zoom(Matrix* image, Matrix* output, float ratio);

Matrix* M_ZoomI(Matrix* image,size_t width, float ratio);

void M_BillinearInterpolation(Matrix* image, Matrix* output);

Matrix* M_IBillinearInterpolation(Matrix* image);

void M_MinTransformation(Matrix* input, Matrix* output, size_t width);

Matrix* M_IMinTransformation(Matrix* input, size_t width);

Matrix* DownScale(const Matrix* input, float ratio);

Matrix* M_Stretch(const Matrix* input, size_t width, size_t height);

Matrix* M_Blur(const Matrix* input);





