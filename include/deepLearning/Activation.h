#pragma once
#include "matrix/Matrix.h"

/*

Activation functions

*/


typedef struct Activation
{
    void (*Compute)(Matrix* m, Matrix* output);
    void (*ComputeDerivative)(Matrix* m, Matrix* output);
    unsigned char activationType;
    
} Activation;

//Sigmoïd
Activation* Sigmoid();
void sigmoid(Matrix* m, Matrix* output);
void sigmoidDerivative(Matrix* m, Matrix* output);

//RELU
Activation* ReLU();
void relu(Matrix* m, Matrix* output);
void reluDerivative(Matrix* m, Matrix* output);

//Tanh
Activation* Softmax();
void softmax(Matrix* m, Matrix* output);
void softmaxDerivative(Matrix* m, Matrix* output);

//Nothing : just return the input
Activation* Linear();
void linear(Matrix* m, Matrix* output);
void linearDerivative(Matrix* m, Matrix* output);

