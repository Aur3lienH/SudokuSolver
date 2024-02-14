#pragma once
#include <stdlib.h>
#include "FCL.h"
#include "Layer.h"
#include "matrix/Matrix.h"


/*

Implementation of a dropout layer : Set to zero some output of the previous layer and send it to the next layer

*/


typedef struct
{
    Layer *layer;
    double dropout_rate;
    Matrix* mask;
    LayerShape* layerShape;
} Dropout;

//Create the dropout layer, used to create the network
Layer* Drop_Create(double dropout_rate);

void Drop_Compile(void* drop, LayerShape* layerShape);
 
Matrix* Drop_FeedForward(void* drop, Matrix* input);

Layer* Drop_Copy(void* drop);

void Drop_Save(void* drop, FILE* file);

Layer* Drop_Load(FILE* file);

void Drop_Free(void* drop);

void Drop_Print(void* drop, int* parametersCount);
