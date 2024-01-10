#pragma once
#include "matrix/Matrix.h"


/*

Loss function used to compute the error of the network

*/


typedef struct
{
    double (*Compute)(const Matrix*, const Matrix* desiredOutput, float* accumulators, float lambda);
    void (*ComputeDerivative)(const Matrix*,const Matrix*,Matrix*);
} Loss;


/// @brief Mean Squared Error 
Loss* MSE_Create();

double MSE_Compute(const Matrix* output, const Matrix* desiredOutput, float* accumulators, float lambda);

void MSE_Derivative(const Matrix* N_Output, const Matrix* N_desiredOutput, Matrix* N_res);

/// @brief Cross Entropy 
Loss* CE_Create();

double CE_Compute(const Matrix* output, const Matrix* desiredOutput, float* accumulators, float lambda);

void CE_Derivative(const Matrix* N_Output, const Matrix* N_desiredOutput, Matrix* N_res);



