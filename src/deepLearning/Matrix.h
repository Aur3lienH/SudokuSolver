#pragma once
#include <stdio.h>
#include "LayerShape.h"


typedef struct Matrix
{
    float* data;

    size_t rows;
    size_t cols;
    size_t dims;

    size_t effectiveRows;
    size_t effectiveCols;
} Matrix;


/// @brief Get the size of the array of the matrix
size_t M_GetSize3D(const Matrix* matrix);

/// @brief Get the size of the array of the matrix
size_t M_GetSize2D(const Matrix* matrix);


/// @brief Print the content of the matrix
void M_Print(const Matrix* m, const char* name);

void M_Print2D(const Matrix* m, const char* name);

/// @brief Print the dimensions of the matrix
void M_Dim(const Matrix* m);

Matrix* SM_Create_3D(size_t rows, size_t cols, size_t dims);

Matrix* SM_Create_2D(size_t rows, size_t cols);

//Create a matrix
Matrix* M_Create_3D(size_t rows, size_t cols, size_t dims);

Matrix* M_Create_2D(size_t rows, size_t cols);

Matrix* M_Create_3D_Data(size_t rows, size_t cols, size_t dims, float* data);

Matrix* M_Create_2D_Data(size_t rows, size_t cols, float* data);

Matrix* M_FillMatrixZero(const Matrix* input,size_t rowsFill,size_t colsFill);

Matrix* M_Init(size_t rows,size_t cols, size_t dims, size_t effectiveRows, size_t effectiveCols);

//Free the memory of the matrix
void M_Free(Matrix* m);

//Get the index of the matrix
size_t M_ConvertIndex(const Matrix* m, size_t rows, size_t cols);

//Multiply the first two matrices and put the ouptut in the third parameter
void M_Mul(const Matrix* a, const Matrix* b, Matrix* output);

//Multiplay all the elements of the matrix by a scalar
void M_ScalarMul(const Matrix* m, const float scalar, Matrix* output);

//Multiplay all the elements of the matrix by all the elements of the second matrix and put the ouptut in the third parameter
void M_LinearMul(const Matrix* a,const Matrix* b, Matrix* output);

float M_GetMax(const Matrix* m);

float M_Get(const Matrix* m, size_t rows, size_t cols);

//Add the first two matrices and put the ouptut in the third parameter
void M_Add(const Matrix* a,const Matrix* b, Matrix* output);

//Apply function to all the elements of the matrix
void M_Apply(float (*func)(float),const Matrix* m, Matrix* output);

//Transpose the first matrix and multiply the two matrices 
void M_Transpose1Mul(const Matrix* a, const Matrix* b, Matrix* output);

//Transpose the second matrix and multiply the two matrices
void M_Transpose2Mul(const Matrix* a,const Matrix* b, Matrix* output);

//Copy the first matrix to the second matrix
void M_Copy(const Matrix* a, Matrix* b);

//Copy the first matrix and return the copy
Matrix* M_CopyI(const Matrix* a);

//Set all the elements of the matrix to a value
void M_Set(Matrix* m, const float value);

//Set all the elements of the matrix to zero
void M_Zero(Matrix* m);

//Create a new matrix transposed
Matrix* M_Transpose(const Matrix* m);

//Check if the two matrices are equal (return 1 if true, 0 if false)
int M_Equals(const Matrix* a, const Matrix* b);

//Save the matrix in a file
void M_Save(const Matrix* m, FILE* file);

//Load the matrix from a file
Matrix* M_Load(FILE* file);

int M_GetMatrixType();

//Set all one element of the matrix to a value
void SM_SetValue(Matrix* m,size_t index, const float value);

void M_SetValue(Matrix* m,size_t rows, size_t cols, const float value);

//Get one element of the matrix
float SM_GetValue(const Matrix* m,size_t index);

//Convolution of the first two matrices and put the ouptut in the third parameter
void M_Convolution(const Matrix* input, const Matrix* filter, Matrix* output);

//Apply a convolution with zero padding on one dimension of the matrix
void M_Convolution_ZeroPad(const Matrix* a, const Matrix* b, Matrix* output);

//Convolution and add the result to the third parameter
void M_Convolution_Add(const Matrix* input, const Matrix* filter, Matrix* output);

//Apply convolution_add on all dimensions of the matrix
void M_Convolution3D_Add(Matrix* input, Matrix* filter, Matrix* output);

//Apply convolution on all the input channels of the matrix with the filter b and put the ouptut in the third parameter
void M_Convolution3D_2D_Add(Matrix* input, Matrix* filter, Matrix* output);

//Apply convolution on all dimensions of the matrix
void M_Convolution3D(Matrix* input, Matrix* filter, Matrix* output);

//Convolution of the first two matrices and put the ouptut in the third parameter with ReLU
void M_ReLU_Convolution(const Matrix* input, const Matrix* filter,float bias, Matrix* deltaActivation, Matrix* output);

//Apply full convolution on all dimensions of the matrix
void M_FullConvolution(Matrix* input, Matrix* filter, Matrix* output);

//Apply full convolution on all dimensions of the matrix
void M_FullConvolution3D(Matrix* a, Matrix* filter, Matrix* output);

Matrix* M_Complete(const Matrix* m, size_t rows, size_t cols);

void M_Rotate180(Matrix* m ,Matrix* output);

void M_Rotate180_3D(Matrix* m ,Matrix* output);

Matrix* M_LS_To_Matrix(const LayerShape* ls);

float M_GetSum(const Matrix* m);

void M_MaxPool(const Matrix* m, Matrix* output, size_t* maxIndexes, size_t size, size_t offset);

void M_MaxPool3D(Matrix* m, Matrix* output, size_t* maxIndexes, size_t size);

void M_AddBiasReLU(Matrix* input, float bias, Matrix* deltaActivation, Matrix* output);

float two_by_two_det(Matrix* m);





