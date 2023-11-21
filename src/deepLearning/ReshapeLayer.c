#include "ReshapeLayer.h"
#include "ConsoleTools.h"


Layer* ReshapeLayer_Create(LayerShape* shape)
{
    ReshapeLayer* reshapeLayer = malloc(sizeof(ReshapeLayer));
    reshapeLayer->layer = malloc(sizeof(Layer));
    reshapeLayer->layer->layerType = 4;
    reshapeLayer->layer->layerShape = shape;
    reshapeLayer->layer->FeedForward = ReshapeLayer_FeedForward;
    reshapeLayer->layer->Process = ReshapeLayer_Process;
    reshapeLayer->layer->BackPropagation = ReshapeLayer_BackPropagate;
    reshapeLayer->layer->AdjustWeigths = ReshapeLayer_UpdateWeights;
    reshapeLayer->layer->Free = ReshapeLayer_Free;
    reshapeLayer->layer->Compile = ReshapeLayer_Compile;
    reshapeLayer->layer->AverageDelta = ReshapeLayer_AverageDeltas;
    reshapeLayer->layer->AddDelta = ReshapeLayer_AddDelta;
    reshapeLayer->layer->Copy = ReshapeLayer_Copy;
    reshapeLayer->layer->Save = ReshapeLayer_Save;
    reshapeLayer->layer->Load = ReshapeLayer_Load;
    reshapeLayer->layer->Print = ReshapeLayer_Print;
    reshapeLayer->layer->layerPtr = reshapeLayer;
    return reshapeLayer->layer;
}

Layer* Flatten_Create()
{
    Layer* flatten = ReshapeLayer_Create(NULL);
    flatten->Compile = Flatten_Compile;
    flatten->layerType = 4;
    return flatten;
}

void Flatten_Compile(void* reshapeLayer, LayerShape* layerShape)
{
    // !!! LEAK OF MEMORY OVER THERE !!!
    ReshapeLayer* reshapeLayerPtr = (ReshapeLayer*)reshapeLayer;
    reshapeLayerPtr->layer->layerShape = LS_Create3D(layerShape->x * layerShape->y * layerShape->z,1,1);
    reshapeLayerPtr->previousShape = *layerShape;
}


void ReshapeLayer_Free(void* reshapeLayer)
{
    free(((ReshapeLayer*)reshapeLayer)->layer);
    free(reshapeLayer);
}


void ReshapeLayer_Compile(void* reshapeLayer, LayerShape* layerShape)
{
    ReshapeLayer* reshapeLayerPtr = (ReshapeLayer*)reshapeLayer;
    reshapeLayerPtr->previousShape = *layerShape;
}

Matrix* ReshapeLayer_FeedForward(void* reshapeLayer, Matrix* input)
{
    ReshapeLayer* reshapeLayerPtr = (ReshapeLayer*)reshapeLayer;
    input->rows = reshapeLayerPtr->layer->layerShape->x;
    input->cols = reshapeLayerPtr->layer->layerShape->y;
    input->dims = reshapeLayerPtr->layer->layerShape->z;
    reshapeLayerPtr->layer->outputs = input;
    return input;
}

Matrix* ReshapeLayer_Process(void* reshapeLayer, Matrix* input)
{
    return ReshapeLayer_FeedForward(reshapeLayer,input);
}


Matrix* ReshapeLayer_BackPropagate(void* reshapeLayer, const Matrix* input, Matrix* delta)
{
    ReshapeLayer* reshapeLayerPtr = (ReshapeLayer*)reshapeLayer;
    delta->rows = reshapeLayerPtr->previousShape.x;
    delta->cols = reshapeLayerPtr->previousShape.y;
    delta->dims = reshapeLayerPtr->previousShape.z;
    Matrix* _input = reshapeLayerPtr->layer->outputs;
    _input->rows = reshapeLayerPtr->previousShape.x;
    _input->cols = reshapeLayerPtr->previousShape.y;
    _input->dims = reshapeLayerPtr->previousShape.z;
    return delta;
}

void ReshapeLayer_UpdateWeights(void* reshapeLayer,float learningRate, float* accumulator, float lambda)
{
    return;
}

void ReshapeLayer_AverageDeltas(void* reshapeLayer, const size_t size)
{
    return;
}

void ReshapeLayer_AddDelta(void* reshapeLayer, void* otherLayer)
{
    return;
}

void ReshapeLayer_Save(void* reshapeLayer, FILE* file)
{
    LS_Save(((ReshapeLayer*)reshapeLayer)->layer->layerShape,file);
}


Layer* ReshapeLayer_Load(FILE* file)
{
    LayerShape* layerShape = LS_Load(file);
    printf("Loaded shape %dx%dx%d\n",layerShape->x,layerShape->y,layerShape->z);
    Layer* layer = ReshapeLayer_Create(layerShape);
    return layer;
}


Layer* ReshapeLayer_Copy(void* reshapeLayer)
{
    ReshapeLayer* reshapeLayerPtr = (ReshapeLayer*)reshapeLayer;
    Layer* layer = ReshapeLayer_Create(reshapeLayerPtr->layer->layerShape);
    layer->Compile = ReshapeLayer_Compile;
    return layer;
}


void ReshapeLayer_Reset(void* reshapeLayer)
{
    ReshapeLayer* reshapeLayerPtr = (ReshapeLayer*)reshapeLayer;
    reshapeLayerPtr->layer->outputs->dims = reshapeLayerPtr->previousShape.z;
    reshapeLayerPtr->layer->outputs->rows = reshapeLayerPtr->previousShape.x;
    reshapeLayerPtr->layer->outputs->cols = reshapeLayerPtr->previousShape.y;
}


void ReshapeLayer_Print(void* reshapeLayer, int* parametersCount)
{
    ReshapeLayer* reshapeLayerPtr = (ReshapeLayer*)reshapeLayer;
    PrintCentered("Reshape Layer");
    printf("Output shape %d: %dx%dx%d\n",*parametersCount,reshapeLayerPtr->layer->layerShape->x,reshapeLayerPtr->layer->layerShape->y,reshapeLayerPtr->layer->layerShape->z);
    printf("Parameters Count: 0\n");
}