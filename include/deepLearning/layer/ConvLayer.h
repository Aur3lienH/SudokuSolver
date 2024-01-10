#pragma once
#include "Layer.h"
#include "../Activation.h"
#include "../Adam.h"


/*

Implementation of a convolutional layer

*/


typedef struct
{
    Layer* layer;
    LayerShape* filterShape;
    Matrix* filters;
    Matrix* rotatedFilters;
    Matrix* deltaActivation;
    Matrix* delta;
    Matrix* newDelta;
    Adam* adam;
    Adam* adamBias;
    float* biases;
    float* deltaBiases;
    
} ConvLayer;

//Create the layer, used to add the layer to the network
Layer* Conv_Create(LayerShape* filterShape);

//Complie
void Conv_Compile(void* layer,LayerShape* layerShape);

//Process the input in training
Matrix* Conv_FeedForward(void* layerPtr, Matrix* input);

//Process the input in testing
Matrix* Conv_Process(void* layerPtr, Matrix* input);

//Compute the gradient of the layer and compute the delta for the previous layer
Matrix* Conv_BackPropagateFully(void* layerPtr, Matrix* input, Matrix* delta);

//Compute the gradient of the layer 
Matrix* Conv_BackPropagateGradient(void* layerPtr, Matrix* input, Matrix* delta);

void Conv_UpdateWeights(void* layerPtr,float learningRate, float* accumulator, float lambda);

void Conv_AverageDeltas(void* layerPtr, const size_t size);

void Conv_AddDelta(void* layerPtr, void* otherLayerPtr);

Layer* Conv_Copy(void* layerPtr);

void Conv_Free(void* layerPtr);

void Conv_Save(void* layerPtr, FILE* file);

Layer* Conv_Load(FILE* file);

void Conv_Print(void* layerPtr, int* parametersCount);