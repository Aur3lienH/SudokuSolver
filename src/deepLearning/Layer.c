#include "Layer.h"
#include "Matrix.h"


void L_AddDelta(void* layer, void* layer2)
{
    return;
}

void L_AverageDelta(void* layer, const size_t size)
{
    return;
}

Matrix* L_BackPropagate(void* layer, const Matrix* input, Matrix* delta)
{
    return delta;
}

void L_Update(void* layer,const float learningRate, float* accumulator, float lambda)
{
    return;
}

Matrix* L_Process(void* layer, Matrix* input)
{
    return input;
}

