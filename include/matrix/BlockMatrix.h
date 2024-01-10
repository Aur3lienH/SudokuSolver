#pragma once
#include "Matrix.h"

/*

Block matrix : a matrix composed of several matrix of same size : BLOCK_SIZE x BLOCK_SIZE
BLOCK_SIZE must be changed on every machine to have better performances, depending on the cache size of the processor

*/



/*

All the operations on block matrix

*/



Matrix* BM_Create_3D(size_t rows, size_t cols, size_t dims);

Matrix* BM_Create_2D(size_t rows, size_t cols);

//BLOCKMATRIX x BLOCKMATRIX = BLOCKMATRIX
void BM_Mul(const Matrix* A, const Matrix* B, Matrix* C);

//BLOCKMATRIX x MATRIX = MATRIX
void BM_M_Mul(const Matrix* A, const Matrix* B, Matrix* C);
void BM_M_SSE_NCOND_Mul(const Matrix* A, const Matrix* B, Matrix* C);
void BM_M_SSE_Mul(const Matrix* A, const Matrix* B, Matrix* C);
void BM_M_AVX_Mul(const Matrix* A, const Matrix* B, Matrix* C);
void BM_M_AVX_NCOND_Mul(const Matrix* A, const Matrix* B, Matrix* C);

//(BLOCKMATRIX)^T x BLOCKMATRIX = BLOCKMATRIX
void BM_Transpose1Mul(const Matrix* A, const Matrix* B, Matrix* C);
//(BLOCKMATRIX)^T x MATRIX = MATRIX
void BM_M_Transpose1Mul(const Matrix* a, const Matrix* b, Matrix* c);
void BM_M_SSE_Transpose1Mul(const Matrix* a, const Matrix* b, Matrix* c);
void BM_M_AVX_Transpose1Mul(const Matrix* a, const Matrix* b, Matrix* c);

//MATRIX x (MATRIX)^T = BLOCKMATRIX
void BM_Transpose2Mul(const Matrix* a, const Matrix* b, Matrix* c);
void BM_M_Transpose2Mul(const Matrix* a, const Matrix* b, Matrix* c);
void BM_M_SSE_Transpose2Mul(const Matrix* a, const Matrix* b, Matrix* c);
void BM_M_AVX_Transpose2Mul(const Matrix* a, const Matrix* b, Matrix* c);
void BM_M_AVX_NCOND_Transpose2Mul(const Matrix* a, const Matrix* b, Matrix* c);

//Print a BLOCKMATRIX
void BM_Print(const Matrix* A);

//Check if a BLOCKMATRIX is equal to a simple Matrix
int BM_M_Equals(const Matrix* blockMatrix, const Matrix* matrix);

//Convert a Matrix to a BLOCK MATRIX
Matrix* M_BM_Convert(const Matrix* a);

//Set the value in a block matrix
void BM_SetValue(Matrix* a,size_t index, float value);

//Get the value in a block matrix
float BM_GetValue(const Matrix* a,size_t index);

//Get the BLOCK_SIZE
size_t SM_GET_BLOCK_SIZE();
