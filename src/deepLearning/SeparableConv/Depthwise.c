#include "Depthwise.h"
#include "../Layer.h"
#include "../Matrix.h"
#include <stdlib.h>

Layer* Conv_Depth_Create(LayerShape* filterShape, unsigned int outputDepth)
{
    Conv_Depth* layer = (Conv_Depth*)malloc(sizeof(Conv_Depth));
    layer->layer = Layer_Create();
    layer->layer->layerType = 5;
    layer->layer->Compile = Conv_Depth_Compile;
    layer->layer->FeedForward = Conv_Depth_FeedForward;
    layer->layer->Process = Conv_Depth_Process;
    layer->layer->BackPropagation = Conv_Depth_Backpropagation;
    layer->layer->AdjustWeigths = Conv_Depth_UpdateWeights;
    layer->layer->Save = Conv_Depth_Save;
    layer->layer->Load = Conv_Depth_Load;
    layer->layer->Copy = Conv_Depth_Copy;
    layer->layer->Free = Conv_Depth_Free;
    layer->filterShape = filterShape;
    layer->outputDepth = outputDepth;
    return (Layer*)layer;
}

void Conv_Depth_Free(void* layerPtr)
{
    Conv_Depth* depthWise = (Conv_Depth*)layerPtr;
    free(depthWise->filterShape);
    free(depthWise);
}

void Conv_Depth_Compile(void* layer,LayerShape* prevLayerShape)
{
    Conv_Depth* depthWise = (Conv_Depth*)layer;
    depthWise->layer->layerShape = LS_Create3D(prevLayerShape->x,prevLayerShape->y,depthWise->outputDepth);
}

Matrix* Conv_Depth_FeedForward(void* layerPtr, Matrix* input)
{
    Conv_Depth* depthWise = (Conv_Depth*)layerPtr;
    depthWise->ConvDepthWise(input,depthWise->filterDepthWise,depthWise->interMat);
    M_PointWiseConvolution(depthWise->interMat,depthWise->filterPointWise,depthWise);
}

Matrix* Conv_Depth_Process(void* layerPtr, Matrix* input)
{
    return Conv_Depth_FeedForward(layerPtr,input);
}

Matrix* Conv_Depth_Backpropagation(void* layerPtr, Matrix* input, Matrix* delta)
{

}


void Conv_Depth_UpdateWeights(void* layerPtr,float learningRate, float* accumulator, float lambda)
{

}


void Conv_Depth_Save(void* layerPtr, FILE* file)
{

}

Layer* Conv_Depth_Load(FILE* file)
{

}


Layer* Conv_Depth_Copy(void* layerPtr)
{

}

void Conv_Depth_AddDelta(void* layerPtr, void* layer2Ptr)
{

}


void Conv_Depth_AverageDelta(void* layerPtr, const size_t size)
{

}

void Conv_Depth_Print(void* layerPtr, int* parametersCount)
{
    
}