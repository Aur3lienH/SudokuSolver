#pragma once
#include "Matrix.h"

Matrix* BM_Create_3D(size_t rows, size_t cols, size_t dims);

Matrix* BM_Create_2D(size_t rows, size_t cols);


void BM_Mul(const Matrix* A, const Matrix* B, Matrix* C);
void BM_M_Mul(const Matrix* A, const Matrix* B, Matrix* C);
void BM_M_SSE_NCOND_Mul(const Matrix* A, const Matrix* B, Matrix* C);
void BM_M_SSE_Mul(const Matrix* A, const Matrix* B, Matrix* C);
void BM_M_AVX_Mul(const Matrix* A, const Matrix* B, Matrix* C);
void BM_M_AVX_NCOND_Mul(const Matrix* A, const Matrix* B, Matrix* C);

void BM_Transpose1Mul(const Matrix* A, const Matrix* B, Matrix* C);
void BM_M_Transpose1Mul(const Matrix* a, const Matrix* b, Matrix* c);
void BM_M_SSE_Transpose1Mul(const Matrix* a, const Matrix* b, Matrix* c);
void BM_M_AVX_Transpose1Mul(const Matrix* a, const Matrix* b, Matrix* c);

void BM_Transpose2Mul(const Matrix* a, const Matrix* b, Matrix* c);
void BM_M_Transpose2Mul(const Matrix* a, const Matrix* b, Matrix* c);
void BM_M_SSE_Transpose2Mul(const Matrix* a, const Matrix* b, Matrix* c);
void BM_M_AVX_Transpose2Mul(const Matrix* a, const Matrix* b, Matrix* c);
void BM_M_AVX_NCOND_Transpose2Mul(const Matrix* a, const Matrix* b, Matrix* c);

void BM_Print(const Matrix* A);

int BM_M_Equals(const Matrix* blockMatrix, const Matrix* matrix);

Matrix* M_BM_Convert(const Matrix* a);

void BM_SetValue(Matrix* a,size_t index, float value);

float BM_GetValue(const Matrix* a,size_t index);

size_t SM_GET_BLOCK_SIZE();
