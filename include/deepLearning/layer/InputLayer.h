#pragma once
#include "matrix/Matrix.h"
#include "deepLearning/Network.h"
#include "Layer.h"
#include "deepLearning/LayerShape.h"
#include <stdio.h>


/*

Input layer : Does not process the input, just send it to the next layer

*/

typedef enum TransformInputType
{
    None,
    Noise
} TransformInputType;


typedef struct
{
    Layer* layer;
    const Matrix* input;
    void (*TransformInput)(const Matrix* input, Matrix* output);
    TransformInputType transformInputType;
} InputLayer;


Layer* I_Create_Transform(size_t neuronsCount, TransformInputType transformInputType);

Layer* I_Create_Transform3D(LayerShape* layerShape, TransformInputType transformInputType);

Layer* I_Create_Transform_Custom(LayerShape* layerShape, void (*TransformInputType)(const Matrix* input, Matrix* output));

Layer* I_Create(size_t neuronsCount);

Layer* I_Create_2D(LayerShape* layerShape);

void I_Free(void* layerPtr);

void I_Compile(void* layerPtr, LayerShape* layerShape);

Matrix* I_FeedForward(void* layerPtr, Matrix* input);

Matrix* I_Process(void* layerPtr, Matrix* input);

void I_Save(void* layerPtr, FILE* file);

Layer* I_Load(FILE* file);

Layer* I_Copy(void* layerPtr);

void I_Print(void* layerPtr, int* parametersCount);