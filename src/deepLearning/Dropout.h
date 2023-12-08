#pragma once
#include <stdlib.h>
#include "FCL.h"
#include "Layer.h"
#include "Matrix.h"


typedef struct
{
    Layer *layer;
    double dropout_rate;
    Matrix* mask;
    LayerShape* layerShape;
} Dropout;


Layer* Drop_Create(double dropout_rate);

void Drop_Compile(void* drop, LayerShape* layerShape);
 
Matrix* Drop_FeedForward(void* drop, Matrix* input);

Layer* Drop_Copy(void* drop);

void Drop_Save(void* drop, FILE* file);

Layer* Drop_Load(FILE* file);

void Drop_Free(void* drop);

void Drop_Print(void* drop);
