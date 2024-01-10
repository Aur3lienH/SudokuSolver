#pragma once
#include <stdlib.h>
#include <stdio.h>

/*

Struct to store the shape of a layer, it's input or output

*/


typedef struct
{
    size_t x;
    size_t y;
    size_t z;
} LayerShape;

//Create a layer shape with the 3 dimensions
LayerShape* LS_Create3D(size_t x, size_t y, size_t z);

//Create a layer shape with 2 dimensions, z is set to 1
LayerShape* LS_Create2D(size_t x, size_t y);

//Create a layer shape with 1 dimension, y and z are set to 1
LayerShape* LS_Create1D(size_t x);

//Free the layer shape
void LS_Free(LayerShape* layerShape);

//Save the layer shape to the file
void LS_Save(LayerShape* layerShape, FILE* file);

//Load the layer shape from the file
LayerShape* LS_Load(FILE* file);

