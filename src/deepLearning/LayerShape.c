#include "deepLearning/LayerShape.h"
#include <stdlib.h>
#include <stdio.h>

LayerShape* LS_Create3D(size_t x, size_t y, size_t z)
{
    LayerShape* layerShape = (LayerShape*)malloc(sizeof(LayerShape));
    layerShape->x = x;
    layerShape->y = y;
    layerShape->z = z;
    return layerShape;
}

LayerShape* LS_Create2D(size_t x, size_t y)
{
    return LS_Create3D(x,y,1);
}

LayerShape* LS_Create1D(size_t x)
{
    return LS_Create3D(x,1,1);
}

void LS_Free(LayerShape* layerShape)
{
    free(layerShape);
}

void LS_Save(LayerShape* layerShape, FILE* file)
{
    fwrite(&layerShape->x,sizeof(size_t),1,file);
    fwrite(&layerShape->y,sizeof(size_t),1,file);
    fwrite(&layerShape->z,sizeof(size_t),1,file);
}

LayerShape* LS_Load(FILE* file)
{
    LayerShape* layerShape = (LayerShape*)malloc(sizeof(LayerShape));
    fread(&layerShape->x,sizeof(size_t),1,file);
    fread(&layerShape->y,sizeof(size_t),1,file);
    fread(&layerShape->z,sizeof(size_t),1,file);
    return layerShape;
}