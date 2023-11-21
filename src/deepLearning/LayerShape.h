#pragma once
#include <stdlib.h>
#include <stdio.h>

typedef struct
{
    size_t x;
    size_t y;
    size_t z;
} LayerShape;


LayerShape* LS_Create3D(size_t x, size_t y, size_t z);

LayerShape* LS_Create2D(size_t x, size_t y);

LayerShape* LS_Create1D(size_t x);

void LS_Free(LayerShape* layerShape);

void LS_Save(LayerShape* layerShape, FILE* file);

LayerShape* LS_Load(FILE* file);

