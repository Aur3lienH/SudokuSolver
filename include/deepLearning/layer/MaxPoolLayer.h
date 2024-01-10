#pragma once
#include "Layer.h"

/*
Max pooling layer : pool only squares
*/

/*
!!! Max Pool layer is doing backpropagation in the feedforward function !!!
This can be optimized for further improvements !!!
*/

typedef struct MaxPoolLayer
{
    Layer* layer;
    LayerShape* filterShape;
    size_t* maxIndexes;
    size_t size;
    Matrix* newDelta;
} MaxPoolLayer;


//Create the layer, used to add the layer to the network
Layer* MaxPool_Create(size_t size);

void MaxPool_Compile(void* layer,LayerShape* layerShape);

Matrix* MaxPool_FeedForward(void* layerPtr, Matrix* input);

Matrix* MaxPool_Process(void* layerPtr, Matrix* input);

Matrix* MaxPool_BackPropagate(void* layerPtr, Matrix* input, Matrix* delta);

void MaxPool_UpdateWeights(void* layerPtr,float learningRate, float* accumulator, float lambda);

void MaxPool_AverageDeltas(void* layerPtr, const size_t size);

void MaxPool_Free(void* layerPtr);

void MaxPool_AddDelta(void* layerPtr, void* layer2);

Layer* MaxPool_Copy(void* layerPtr);

void MaxPool_Save(void* layerPtr, FILE* file);

Layer* MaxPool_Load(FILE* file);

void MaxPool_Print(void* layerPtr, int* parametersCount);