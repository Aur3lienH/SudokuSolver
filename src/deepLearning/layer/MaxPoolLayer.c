#include "deepLearning/layer/MaxPoolLayer.h"
#include "matrix/Matrix.h"
#include "deepLearning/LayerShape.h"
#include "tools/ConsoleTools.h"

Layer* MaxPool_Create(size_t size)
{
    MaxPoolLayer* maxPoolLayer = (MaxPoolLayer*)malloc(sizeof(MaxPoolLayer));
    maxPoolLayer->layer = (Layer*)malloc(sizeof(Layer));
    maxPoolLayer->layer->layerType = 3;
    maxPoolLayer->filterShape = LS_Create2D(size,size);
    maxPoolLayer->layer->FeedForward = MaxPool_FeedForward;
    maxPoolLayer->layer->Process = MaxPool_Process;
    maxPoolLayer->layer->BackPropagation = MaxPool_BackPropagate;
    maxPoolLayer->layer->AdjustWeigths = MaxPool_UpdateWeights;
    maxPoolLayer->layer->Free = MaxPool_Free;
    maxPoolLayer->layer->Compile = MaxPool_Compile;
    maxPoolLayer->layer->AverageDelta = MaxPool_AverageDeltas;
    maxPoolLayer->layer->AddDelta = MaxPool_AddDelta;
    maxPoolLayer->layer->Copy = MaxPool_Copy;
    maxPoolLayer->layer->Save = MaxPool_Save;
    maxPoolLayer->layer->Load = MaxPool_Load;
    maxPoolLayer->layer->Print = MaxPool_Print;
    maxPoolLayer->layer->layerPtr = maxPoolLayer;
    maxPoolLayer->size = size;
    return maxPoolLayer->layer;
}


void MaxPool_Free(void* layer)
{
    MaxPoolLayer* maxPoolLayer = (MaxPoolLayer*)layer;
    M_Free(maxPoolLayer->layer->outputs);
    free(maxPoolLayer);
}


void MaxPool_Compile(void* layer,LayerShape* layerShape)
{
    MaxPoolLayer* maxPoolLayer = (MaxPoolLayer*)layer;
    maxPoolLayer->layer->layerShape = LS_Create3D(layerShape->x / maxPoolLayer->filterShape->x,layerShape->y / maxPoolLayer->filterShape->y,layerShape->z);
    maxPoolLayer->layer->outputs = M_Create_3D(maxPoolLayer->layer->layerShape->x,maxPoolLayer->layer->layerShape->y,maxPoolLayer->layer->layerShape->z);
    maxPoolLayer->maxIndexes = (size_t*)malloc(sizeof(size_t) * M_GetSize3D(maxPoolLayer->layer->outputs));
    maxPoolLayer->newDelta = M_Create_3D(layerShape->x,layerShape->y,layerShape->z);
}


Matrix* MaxPool_FeedForward(void* layerPtr, Matrix* input)
{
    MaxPoolLayer* maxPoolLayer = (MaxPoolLayer*)layerPtr;
    M_MaxPool3D(input,maxPoolLayer->layer->outputs,maxPoolLayer->maxIndexes,maxPoolLayer->size);
    return maxPoolLayer->layer->outputs;
}


Matrix* MaxPool_Process(void* layerPtr, Matrix* input)
{
    return MaxPool_FeedForward(layerPtr,input);
}


void ConvertDelta(Matrix* delta, size_t* maxIndexes, Matrix* newDelta, size_t size)
{
    M_Zero(newDelta);
    for (size_t i = 0; i < size; i++)
    {
        printf("%ld \n",maxIndexes[i]);
        newDelta->data[maxIndexes[i]] = delta->data[i];
    }
}


Matrix* MaxPool_BackPropagate(void* layerPtr, Matrix* input, Matrix* delta)
{
    MaxPoolLayer* maxPoolLayer = (MaxPoolLayer*)layerPtr;
    M_Zero(maxPoolLayer->newDelta);
    size_t size = M_GetSize3D(maxPoolLayer->layer->outputs); 
    for (size_t i = 0; i < size; i++)
    {
        maxPoolLayer->newDelta->data[maxPoolLayer->maxIndexes[i]] = delta->data[i];
    }
    
    return maxPoolLayer->newDelta;
}

Layer* MaxPool_Copy(void* layerPtr)
{
    MaxPoolLayer* maxPoolLayer = (MaxPoolLayer*)layerPtr;
    return MaxPool_Create(maxPoolLayer->size);
}

void MaxPool_UpdateWeights(void* layerPtr,float learningRate, float* accumulator, float lambda)
{
    return;
}

void MaxPool_AverageDeltas(void* layerPtr, const size_t size)
{
    return;
}

void MaxPool_AddDelta(void* layerPtr, void* layer2)
{
    return ;
}


void MaxPool_Save(void* layerPtr, FILE* file)
{
    MaxPoolLayer* maxPoolLayer = (MaxPoolLayer*)layerPtr;
    fwrite(&maxPoolLayer->size,sizeof(size_t),1,file);
}


Layer* MaxPool_Load(FILE* file)
{
    size_t size;
    fread(&size,sizeof(size_t),1,file);
    return MaxPool_Create(size);
}

void MaxPool_Print(void* layerPtr, int* parametersCount)
{
    MaxPoolLayer* maxPoolLayer = (MaxPoolLayer*)layerPtr;
    PrintCentered("Max Pooling Layer");
    printf("Size: %ld\n",maxPoolLayer->size);
    printf("Output Shape: %ld %ld %ld\n",maxPoolLayer->layer->layerShape->x,maxPoolLayer->layer->layerShape->y,maxPoolLayer->layer->layerShape->z);
    printf("Parameters Count: 0\n");
    printf("\n");
}
