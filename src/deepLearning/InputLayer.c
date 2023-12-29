#include "InputLayer.h"
#include "Network.h"
#include "Matrix.h"
#include "RandomInit.h"
#include "LayerShape.h"
#include "ConsoleTools.h"
#include <err.h>


Layer* I_Create_Transform(size_t neuronsCount, TransformInputType transformInputType)
{
    return I_Create_Transform3D(LS_Create1D(neuronsCount),transformInputType);
}

Layer* I_Create_Transform3D(LayerShape* layerShape, TransformInputType transformInputType)
{
    InputLayer* inputLayer = (InputLayer*)malloc(sizeof(InputLayer));
    inputLayer->transformInputType = transformInputType;
    switch(transformInputType)
    {
        case None:
            inputLayer->TransformInput = M_Copy;
            break;
        case Noise:
            inputLayer->TransformInput = M_AddNoise;
            break;
    }
    inputLayer->layer = (Layer*)malloc(sizeof(Layer));
    inputLayer->layer->Copy = I_Copy;
    inputLayer->layer->Load = I_Load;
    inputLayer->layer->Save = I_Save;
    inputLayer->layer->outputs = M_LS_To_Matrix(layerShape);
    inputLayer->layer->FeedForward = I_FeedForward;
    inputLayer->layer->Process = I_Process;
    inputLayer->layer->Free = I_Free;
    inputLayer->layer->Compile = I_Compile;
    inputLayer->layer->Print = I_Print;
    inputLayer->layer->layerPtr = inputLayer;
    inputLayer->layer->layerShape = layerShape;
    inputLayer->layer->layerType = 0;
    inputLayer->layer->BackPropagation = NULL;
    inputLayer->layer->AdjustWeigths = NULL;
    inputLayer->layer->AddDelta = NULL;
    inputLayer->layer->AverageDelta = NULL;
    inputLayer->layer->inputs = NULL;

    return inputLayer->layer;
}

Layer* I_Create(size_t neuronsCount)
{
    return I_Create_Transform(neuronsCount,None);
}

Layer* I_Create_2D(LayerShape* layerShape)
{
    return I_Create_Transform3D(layerShape,None);
}

Layer* I_Create_Transform_Custom(LayerShape* layerShape, void (*Custom)(const Matrix* input, Matrix* output))
{
    Layer* inputLayer = I_Create_Transform3D(layerShape,None);
    InputLayer* inputLayerPtr = (InputLayer*)inputLayer->layerPtr;
    inputLayerPtr->TransformInput = Custom;
    return inputLayer;
}

void I_Free(void* layerPtr)
{
    InputLayer* inputLayer = (InputLayer*)layerPtr;
    LS_Free(inputLayer->layer->layerShape);
    free(inputLayer);
}

void I_Compile(void* layerPtr, LayerShape* layerShape)
{
    
}

Matrix* I_Process(void* layerPtr, Matrix* input)
{
    InputLayer* inputLayer = (InputLayer*)layerPtr;
    M_Copy(input,inputLayer->layer->outputs);
    return inputLayer->layer->outputs;
}

Matrix* I_FeedForward(void* layerPtr, Matrix* input)
{
    InputLayer* inputLayer = (InputLayer*)layerPtr;
    inputLayer->TransformInput(input,inputLayer->layer->outputs);

    return inputLayer->layer->outputs;
}


void I_Save(void* layerPtr, FILE* file)
{
    InputLayer* inputLayer = (InputLayer*)layerPtr;
    LS_Save(inputLayer->layer->layerShape,file);
    fwrite(&inputLayer->layer->layerType,sizeof(unsigned char),1,file);
}

Layer* I_Load(FILE* file)
{
    LayerShape* layerShape = LS_Load(file);
    unsigned char layerType;
    fread(&layerType,sizeof(unsigned char),1,file);

    switch (layerType)
    {
        case 0:
            return I_Create_Transform3D(layerShape,None);
        case 1:
            return I_Create_Transform3D(layerShape,Noise);
    }
    return NULL;
    
}

Layer* I_Copy(void* layerPtr)
{
    InputLayer* inputLayer = (InputLayer*)layerPtr;
    return I_Create_Transform3D(inputLayer->layer->layerShape,inputLayer->transformInputType);
}

void I_Print(void* layerPtr, int* parametersCount)
{
    InputLayer* inputLayer = (InputLayer*)layerPtr;
    PrintCentered("Input Layer");
    printf("Shape: %li x %li x %li\n",inputLayer->layer->layerShape->x,inputLayer->layer->layerShape->y,inputLayer->layer->layerShape->z);
    (*parametersCount)++;
}