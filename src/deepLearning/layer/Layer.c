#include "deepLearning/layer/Layer.h"
#include "matrix/Matrix.h"


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


void L_Free(Layer* layer)
{
    free(layer->layerShape);
    free(layer);
}

