#include "Activation.h"
#include <math.h>
#include <stdlib.h>


float sigmoidFunc(float x)
{
    return 1 / (1 + expf(-x));
}

void sigmoid(Matrix* m, Matrix* output)
{
    M_Apply(sigmoidFunc, m, output);
}

float sigmoidDerivativeFunc(float x)
{
    return x * (1 - x);
}

void sigmoidDerivative(Matrix* m, Matrix* output)
{
    M_Apply(sigmoidDerivativeFunc, m, output);
}

float reluFunc(float x)
{
    return x > 0 ? x : 0;
}

void relu(Matrix* m, Matrix* output)
{
    M_Apply(reluFunc, m, output);
}

float reluDerivativeFunc(float x)
{
    return x > 0 ? 1 : 0;
}

void reluDerivative(Matrix* m, Matrix* output)
{
    M_Apply(reluDerivativeFunc, m, output);
}

void softmax(Matrix* m, Matrix* output)
{
    size_t size = m->effectiveCols * m->effectiveRows;
    double sum = 0;
    double max = m->data[0];
    for (size_t i = 0; i < size; i++)
    {
        if (m->data[i] > max)
        {
            max = m->data[i];
        }
    }

    for (size_t i = 0; i < size; i++)
    {
        sum += exp(m->data[i] - max);
    }
    for (size_t i = 0; i < size; i++)
    {
        output->data[i] = exp(m->data[i] - max) / sum;
    }
}


void softmaxDerivative(Matrix* m, Matrix* output)
{
    size_t size = output->effectiveCols * output->effectiveRows;
    
    for (size_t i = 0; i < size; i++)
    {
        output->data[i] = 1;
    }
}


Activation* Sigmoid()
{
    Activation* res = (Activation*)malloc(sizeof(Activation));
    res->Compute = sigmoid;
    res->ComputeDerivative = sigmoidDerivative;
    res->activationType = 0;
    return res;
}

Activation* ReLU()
{
    Activation* res = (Activation*)malloc(sizeof(Activation));
    res->Compute = relu;
    res->ComputeDerivative = reluDerivative;
    res->activationType = 1;
    return res;
}

Activation* Softmax()
{
    Activation* res = (Activation*)malloc(sizeof(Activation));
    res->Compute = softmax;
    res->ComputeDerivative = softmaxDerivative;
    res->activationType = 2;
    return res;
}