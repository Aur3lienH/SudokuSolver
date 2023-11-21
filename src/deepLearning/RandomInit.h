#pragma once
#include "Matrix.h"


/// @brief He Uniform Initialization (Initialization of weights for ReLU activation function)
void HeUniform(Matrix* m);

void M_HeUniform(Matrix* m);
void M_HeUniformConv(Matrix* m, size_t previousLayerdDim);

void M_AddNoise(const Matrix* m, Matrix* output);