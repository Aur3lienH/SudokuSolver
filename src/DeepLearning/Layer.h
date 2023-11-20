#pragma once

#include "Matrix.h"
#include "LayerShape.h"


typedef struct Layer
{
    Matrix* (*FeedForward)(void* layer, Matrix* input);
    Matrix* (*Process)(void* layer, Matrix* input);
    Matrix* (*BackPropagation)(void* layer, const Matrix* input, Matrix* delta);
    void (*AdjustWeigths)(void* layer,const float learningRate, float* accumulator, float lambda);
    void (*Compile)(void* layer, LayerShape*);
    void (*Free)(void* layer);
    void (*Save)(void* layer, FILE* file);
    void (*Print)(void* layer, int* parameterCount);
    struct Layer* (*Load) (FILE* file);
    struct Layer* (*Copy) (void* layer);
    void (*AddDelta)(void* layer, void* layer2);
    void (*AverageDelta)(void* layer, const size_t size);
    Matrix* inputs;
    Matrix* outputs;
    LayerShape* layerShape;
    void* layerPtr;
    unsigned char layerType;
    size_t layerPos;
} Layer;



void L_AddDelta(void* layer, void* layer2);

void L_AverageDelta(void* layer, const size_t size);

Matrix* L_BackPropagate(void* layer, const Matrix* input, Matrix* delta);

void L_Update(void* layer,const float learningRate, float* accumulator, float lambda);

const Matrix* L_Process(void* layer, const Matrix* input);