#pragma once
#include <stdlib.h>

/*

Simple implementation of the adam optimizer : does not understand anything here but it WORKS !

*/




typedef struct 
{
    
} Optimizer;



typedef struct
{
    float* momentum1;
    float* momentum2;
    float beta1;
    float beta2;
    float adjbeta1;
    float adjbeta2;
    float epsilon;
    size_t size;

} Adam;

/// @brief Create a adam optimizer
/// @param beta1 
/// @param beta2 
/// @param epsilon 
/// @param size The size of the array of parameters to update
/// @return An adam optimizer
Adam* Adam_Create(float beta1, float beta2, float epsilon, size_t size);

Adam* Adam_Create_D(size_t size);

/// @brief Update the parameters with the gradients
void Adam_Update(Adam* adam, float* weights, float* gradients, float learning_rate);

void Adam_Update_L2(Adam* adam, float* weights, float* gradients, float learning_rate, float* accumulator, float lambda);

/// @brief Free the adam optimizer
void Adam_Free(Adam* adam);

 