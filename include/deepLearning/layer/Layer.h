#pragma once

#include "matrix/Matrix.h"
#include "deepLearning/LayerShape.h"


/*

Shape of every layer
All the functions pointer are mandatory when creating a layer, otherwise it will cause segfaults
*/



typedef struct Layer
{
    Matrix* (*FeedForward)(void* layer, Matrix* input); //MANDATORY
    Matrix* (*Process)(void* layer, Matrix* input);     //MANDATORY
    Matrix* (*BackPropagation)(void* layer, Matrix* input, Matrix* delta); //MANDATORY
    void (*AdjustWeigths)(void* layer,const float learningRate, float* accumulator, float lambda); //MANDATORY
    void (*Compile)(void* layer, LayerShape*); //MANDATORY
    void (*Free)(void* layer); //MANDATORY
    void (*Save)(void* layer, FILE* file); //MANDATORY
    void (*Print)(void* layer, int* parameterCount); //MANDATORY
    struct Layer* (*Load) (FILE* file); //MANDATORY
    struct Layer* (*Copy) (void* layer); //MANDATORY
    void (*AddDelta)(void* layer, void* layer2); //MANDATORY
    void (*AverageDelta)(void* layer, const size_t size); //MANDATORY
    Matrix* inputs;
    Matrix* outputs;
    LayerShape* layerShape;
    void* layerPtr;
    unsigned char layerType;
    size_t layerPos;
} Layer;


//Default functions to do nothing
void L_AddDelta(void* layer, void* layer2);
//Default functions to do nothing
void L_AverageDelta(void* layer, const size_t size);
//Default functions to do nothing
Matrix* L_BackPropagate(void* layer, Matrix* input, Matrix* delta);
//Default functions to do nothing
void L_Update(void* layer,const float learningRate, float* accumulator, float lambda);
//Default functions to do nothing
Matrix* L_Process(void* layer, Matrix* input);
//Default functions to do nothing
void L_Free(Layer* layer);