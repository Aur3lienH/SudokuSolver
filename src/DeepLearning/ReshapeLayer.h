#pragma once
#include "Layer.h"


typedef struct ReshapeLayer
{
    Layer* layer;
    LayerShape previousShape;
} ReshapeLayer;


Layer* ReshapeLayer_Create(LayerShape* nextShape);

Layer* Flatten_Create();

void Flatten_Compile(void* reshapeLayer, LayerShape* layerShape);

void ReshapeLayer_Free(void* reshapeLayer);

void ReshapeLayer_Compile(void* reshapeLayer, LayerShape* layerShape);

Matrix* ReshapeLayer_FeedForward(void* reshapeLayer, Matrix* input);

Matrix* ReshapeLayer_Process(void* reshapeLayer, Matrix* input);

Matrix* ReshapeLayer_BackPropagate(void* reshapeLayer, const Matrix* input, Matrix* delta);

void ReshapeLayer_UpdateWeights(void* reshapeLayer,float learningRate, float* accumulator, float lambda);

void ReshapeLayer_AverageDeltas(void* reshapeLayer, const size_t size);

void ReshapeLayer_AddDelta(void* reshapeLayer, void* otherLayer);

void ReshapeLayer_Save(void* reshapeLayer, FILE* file);

Layer* ReshapeLayer_Load(FILE* file);

Layer* ReshapeLayer_Copy(void* reshapeLayer);

void ReshapeLayer_Reset(void* reshapeLayer);

void ReshapeLayer_Print(void* reshapeLayer, int* parametersCount);
