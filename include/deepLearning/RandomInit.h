#pragma once
#include "matrix/Matrix.h"


/// @brief He Uniform Initialization (Initialization of weights for ReLU activation function)
void HeUniform(Matrix* m);

void M_HeUniform(Matrix* m);

//Specialized for convolutional layers
void M_HeUniformConv(Matrix* m, size_t previousLayerdDim);

//Add noise to an image
void M_AddNoise(const Matrix* m, Matrix* output);