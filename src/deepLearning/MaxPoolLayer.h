#pragma once
#include "Layer.h"


typedef struct MaxPoolLayer
{
    Layer* layer;
    LayerShape* filterShape;
    size_t* maxIndexes;
    size_t size;
    Matrix* newDelta;
} MaxPoolLayer;

/*
!!! Max Pool layer is doing backpropagation in the feedforward function !!!
This can be optimized for further improvements !!!
*/



Layer* MaxPool_Create(size_t size);

void MaxPool_Compile(void* layer,LayerShape* layerShape);

Matrix* MaxPool_FeedForward(void* layerPtr, Matrix* input);

Matrix* MaxPool_Process(void* layerPtr, Matrix* input);

Matrix* MaxPool_BackPropagate(void* layerPtr, const Matrix* input, Matrix* delta);

void MaxPool_UpdateWeights(void* layerPtr,float learningRate, float* accumulator, float lambda);

void MaxPool_AverageDeltas(void* layerPtr, const size_t size);

void MaxPool_Free(void* layerPtr);

void MaxPool_AddDelta(void* layerPtr, void* layer2);

Layer* MaxPool_Copy(void* layerPtr);

void MaxPool_Save(void* layerPtr, FILE* file);

Layer* MaxPool_Load(FILE* file);

void MaxPool_Print(void* layerPtr, int* parametersCount);