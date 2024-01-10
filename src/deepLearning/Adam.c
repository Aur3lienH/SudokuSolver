#include "deepLearning/Adam.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>


Adam* Adam_Create(float beta1, float beta2, float epsilon, size_t size)
{
    Adam* adam = (Adam*)malloc(sizeof(Adam));
    adam->beta1 = beta1;
    adam->beta2 = beta2;
    adam->adjbeta1 = 1;
    adam->adjbeta2 = 1;
    adam->epsilon = epsilon;
    adam->size = size;
    adam->momentum1 = (float*)malloc(sizeof(float) * size);
    adam->momentum2 = (float*)malloc(sizeof(float) * size);
    for (size_t i = 0; i < size; i++)
    {
        adam->momentum1[i] = 0;
        adam->momentum2[i] = 0;
    }
    
    
    return adam;
}

Adam* Adam_Create_D(size_t size)
{
    return Adam_Create(0.9,0.999,1e-7,size);
}

void Adam_Update(Adam* adam, float* parameters, float* gradients, float learningRate)
{
    adam->adjbeta1 *= adam->beta1;
    adam->adjbeta2 *= adam->beta2;
    for (size_t i = 0; i < adam->size; i++)
    {
        adam->momentum1[i] = adam->beta1 * adam->momentum1[i] + (1 - adam->beta1) * gradients[i];
        adam->momentum2[i] = adam->beta2 * adam->momentum2[i] + (1 - adam->beta2) * gradients[i] * gradients[i];

        float momentum1Corrected = adam->momentum1[i] / (1 - adam->adjbeta1);
        float momentum2Corrected = adam->momentum2[i] / (1 - adam->adjbeta2);
        
        parameters[i] -= learningRate * momentum1Corrected / (sqrtf(momentum2Corrected) + adam->epsilon);
        //printf("%f\n",learningRate * momentum1Corrected / (sqrtf(momentum2Corrected) + adam->epsilon));
        //parameters[i] -= learningRate * gradients[i];
    }

}

void Adam_Update_L2(Adam* adam, float* parameters, float* gradients, float learningRate, float* accumulator, float lambda)
{
    adam->adjbeta1 *= adam->beta1;
    adam->adjbeta2 *= adam->beta2;
    for (size_t i = 0; i < adam->size; i++)
    {
        gradients[i] += lambda * parameters[i];
        adam->momentum1[i] = adam->beta1 * adam->momentum1[i] + (1 - adam->beta1) * gradients[i];
        adam->momentum2[i] = adam->beta2 * adam->momentum2[i] + (1 - adam->beta2) * gradients[i] * gradients[i];

        float momentum1Corrected = adam->momentum1[i] / (1 - adam->adjbeta1);
        float momentum2Corrected = adam->momentum2[i] / (1 - adam->adjbeta2);
        
        parameters[i] -= learningRate * momentum1Corrected / (sqrtf(momentum2Corrected) + adam->epsilon);
        //printf("%f\n",learningRate * momentum1Corrected / (sqrtf(momentum2Corrected) + adam->epsilon));
        //parameters[i] -= learningRate * gradients[i];
    }
}

void Adam_Free(Adam* adam)
{
    free(adam->momentum1);
    free(adam->momentum2);
    free(adam);
}