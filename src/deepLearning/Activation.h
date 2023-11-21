#pragma once
#include "Matrix.h"

typedef struct Activation
{
    void (*Compute)(Matrix* m, Matrix* output);
    void (*ComputeDerivative)(Matrix* m, Matrix* output);
    unsigned char activationType;
    
} Activation;


Activation* Sigmoid();
void sigmoid(Matrix* m, Matrix* output);
void sigmoidDerivative(Matrix* m, Matrix* output);

Activation* ReLU();
void relu(Matrix* m, Matrix* output);
void reluDerivative(Matrix* m, Matrix* output);

Activation* Softmax();
void softmax(Matrix* m, Matrix* output);
void softmaxDerivative(Matrix* m, Matrix* output);

