#include "ConvLayer.h"
#include "Layer.h"
#include "Activation.h"
#include "Matrix.h"
#include "Adam.h"
#include "RandomInit.h"
#include "ConsoleTools.h"
#include <string.h>
#include <stdlib.h>


Layer* Conv_Create(LayerShape* layerShape)
{

    ConvLayer* convLayer = (ConvLayer*)malloc(sizeof(ConvLayer));
    convLayer->layer = (Layer*)malloc(sizeof(Layer));
    convLayer->layer->layerType = 5;
    convLayer->filterShape = layerShape;
    convLayer->layer->FeedForward = Conv_FeedForward;
    convLayer->layer->Process = Conv_Process;
    convLayer->layer->AdjustWeigths = Conv_UpdateWeights;
    convLayer->layer->Free = Conv_Free;
    convLayer->layer->Compile = Conv_Compile;
    convLayer->layer->AverageDelta = Conv_AverageDeltas;
    convLayer->layer->AddDelta = Conv_AddDelta;
    convLayer->layer->Copy = Conv_Copy;
    convLayer->layer->Save = Conv_Save;
    convLayer->layer->Load = Conv_Load;
    convLayer->layer->Print = Conv_Print;
    convLayer->layer->layerPtr = convLayer;
    convLayer->filters = NULL;
    convLayer->biases = NULL;
    return convLayer->layer;
}

void Conv_Free(void* layer)
{
    ConvLayer* convLayer = (ConvLayer*)layer;
    M_Free(convLayer->layer->outputs);
    M_Free(convLayer->filters);
    M_Free(convLayer->rotatedFilters);
    M_Free(convLayer->deltaActivation);
    M_Free(convLayer->delta);
    M_Free(convLayer->newDelta);
    free(convLayer->biases);
    free(convLayer->deltaBiases);
    free(convLayer);
}

void Conv_Compile(void* layer,LayerShape* previousLayerShape)
{
    ConvLayer* convLayer = (ConvLayer*)layer;
    size_t dataFilterSize = convLayer->filterShape->x * convLayer->filterShape->y * convLayer->filterShape->z;
    convLayer->layer->layerShape = LS_Create3D(previousLayerShape->x - convLayer->filterShape->x + 1,previousLayerShape->y - convLayer->filterShape->y + 1,convLayer->filterShape->z);
    convLayer->adam = Adam_Create_D(dataFilterSize);
    convLayer->adamBias = Adam_Create_D(convLayer->layer->layerShape->z);
    if(convLayer->filters == NULL)
    {
        convLayer->filters = M_Create_3D(convLayer->filterShape->x,convLayer->filterShape->y,convLayer->filterShape->z);
        M_HeUniformConv(convLayer->filters,previousLayerShape->z);
    }


    convLayer->rotatedFilters = M_Create_3D(convLayer->filterShape->x,convLayer->filterShape->y,convLayer->filterShape->z);
    size_t x = convLayer->layer->layerShape->x;
    size_t y = convLayer->layer->layerShape->y;
    size_t z = convLayer->layer->layerShape->z;
    
    convLayer->layer->outputs = M_Create_3D(x,y,z);
    convLayer->deltaActivation = M_Create_3D(x,y,z);
    convLayer->delta = M_Create_3D(convLayer->filterShape->x,convLayer->filterShape->y,convLayer->filterShape->z);
    convLayer->newDelta = M_Create_3D(previousLayerShape->x,previousLayerShape->y,previousLayerShape->z);

    if(convLayer->biases == NULL)
    {
        convLayer->biases = (float*)calloc(z,sizeof(float) * z);
        
    }
    convLayer->deltaBiases = (float*)calloc(z,sizeof(float) * z);

    if(convLayer->layer->layerPos <= 1)
    {
        convLayer->layer->BackPropagation = Conv_BackPropagateGradient;
    }
    else
    {
        convLayer->layer->BackPropagation = Conv_BackPropagateFully;
    }
    
}


void ForwardPass(Matrix* input, Matrix* filters,Matrix* deltaActivation, Matrix* outputs, float* bias)
{
    float* convLayerArray = outputs->data;
    float* filterArray = filters->data;
    float* deltaArray = deltaActivation->data;
    M_Zero(outputs);
    for (size_t i = 0; i < filters->dims; i++)
    {
        M_Convolution3D_2D_Add(input,filters,outputs);
        M_AddBiasReLU(outputs,bias[i],deltaActivation,outputs);
        outputs->data += M_GetSize2D(outputs);
        filters->data += M_GetSize2D(filters);
        deltaActivation->data += M_GetSize2D(deltaActivation);
    }

    filters->data = filterArray;
    outputs->data = convLayerArray;
    deltaActivation->data = deltaArray;

}




Matrix* Conv_FeedForward(void* layerPtr, Matrix* input)
{
    ConvLayer* convLayer = (ConvLayer*)layerPtr;
    ForwardPass(input, convLayer->filters,convLayer->deltaActivation,convLayer->layer->outputs,convLayer->biases);

    
    return convLayer->layer->outputs;
}

Matrix* Conv_Process(void* layerPtr, Matrix* input)
{
    return Conv_FeedForward(layerPtr, input);
}



Matrix* Conv_BackPropagateFully(void* layerPtr, const Matrix* input, Matrix* delta)
{
    ConvLayer* convLayer = (ConvLayer*)layerPtr;
    //reluDerivative(convLayer->layer->outputs,convLayer->deltaActivation);
    M_LinearMul(convLayer->deltaActivation, delta, convLayer->deltaActivation);
    M_Rotate180_3D(convLayer->filters, convLayer->rotatedFilters);
    M_FullConvolution3D(convLayer->rotatedFilters,convLayer->deltaActivation, convLayer->newDelta);
    
    M_Convolution3D_Add(input,convLayer->deltaActivation, convLayer->delta);
    
    return convLayer->newDelta;
}

Matrix* Conv_BackPropagateGradient(void* layerPtr, const Matrix* input, Matrix* delta)
{
    ConvLayer* convLayer = (ConvLayer*)layerPtr;
    //reluDerivative(convLayer->layer->outputs,convLayer->deltaActivation);
    M_LinearMul(convLayer->deltaActivation, delta, convLayer->deltaActivation);
    //M_Rotate180_3D(convLayer->filters, convLayer->rotatedFilters);
    //M_FullConvolution3D(convLayer->rotatedFilters,convLayer->deltaActivation, convLayer->newDelta);+
    M_Convolution3D_Add(input,convLayer->deltaActivation, convLayer->delta);
    return convLayer->newDelta;

}

void Conv_UpdateWeights(void* layerPtr,float learningRate, float* accumulator, float lambda)
{
    ConvLayer* convLayer = (ConvLayer*)layerPtr;
    
    //M_Print(convLayer->delta);

    Adam_Update_L2(convLayer->adam, convLayer->filters->data, convLayer->delta->data, learningRate, accumulator, lambda);
    

    for (size_t i = 0; i < convLayer->delta->dims; i++)
    {
        float sum = 0;
        size_t count = M_GetSize2D(convLayer->delta);
        for (size_t j = 0; j < count; j++)
        {
            sum += convLayer->delta->data[i * count + j];
        }
        convLayer->deltaBiases[i] = sum;
    }
    
    
    Adam_Update(convLayer->adamBias, convLayer->biases, convLayer->deltaBiases, learningRate);

    M_Zero(convLayer->delta);
    memset(convLayer->deltaBiases,0,sizeof(float) * convLayer->delta->dims);
}


void Conv_AddDelta(void* layerPtr, void* otherLayerPtr)
{
    ConvLayer* convLayer = (ConvLayer*)layerPtr;
    ConvLayer* otherConvLayer = (ConvLayer*)otherLayerPtr;
    M_Add(convLayer->delta,otherConvLayer->delta,convLayer->delta);
    for (size_t i = 0; i < convLayer->delta->dims; i++)
    {
        convLayer->deltaBiases[i] += otherConvLayer->deltaBiases[i];
    }
    M_Zero(otherConvLayer->delta);
    memset(otherConvLayer->deltaBiases,0,sizeof(float) * otherConvLayer->delta->dims);
}


void Conv_AverageDeltas(void* layerPtr, const size_t size)
{
    ConvLayer* convLayer = (ConvLayer*)layerPtr;
    M_ScalarMul(convLayer->delta,1.0f / size,convLayer->delta);


    for (size_t i = 0; i < convLayer->layer->layerShape->z; i++)
    {
        convLayer->deltaBiases[i] /= size;
    }

    
}


Layer* Conv_Copy(void* layerPtr)
{
    ConvLayer* convLayer = (ConvLayer*)layerPtr;
    Layer* layer = Conv_Create(convLayer->filterShape);
    ConvLayer* newConvLayer = (ConvLayer*)layer->layerPtr;
    Matrix* newFilters = M_Create_3D_Data(convLayer->filters->rows,convLayer->filters->cols,convLayer->filters->dims,convLayer->filters->data);
    newConvLayer->filters = newFilters;
    newConvLayer->biases = convLayer->biases;
    return layer;
}


void Conv_Save(void* layerPtr, FILE* file)
{
    ConvLayer* convLayer = (ConvLayer*)layerPtr;
    LS_Save(convLayer->filterShape,file);
    M_Save(convLayer->filters,file);
    fwrite(convLayer->biases,sizeof(float),convLayer->layer->layerShape->z,file);
}



Layer* Conv_Load(FILE* file)
{
    LayerShape* filterShape = LS_Load(file);
    Layer* layer = Conv_Create(filterShape);
    ConvLayer* convLayer = (ConvLayer*)layer->layerPtr;
    Matrix* filters = M_Load(file);
    convLayer->filters = filters;
    convLayer->biases = (float*)malloc(sizeof(float) * filterShape->z);
    if(convLayer->biases == NULL)
    {
        printf("Error Allocating Memory\n");
        exit(1);
    }
    fread(convLayer->biases,sizeof(float),filterShape->z,file);
    return convLayer->layer;
}


void Conv_Print(void* layerPtr, int* parametersCount)
{
    ConvLayer* convLayer = (ConvLayer*)layerPtr;
    int convParameters = convLayer->filters->rows * convLayer->filters->cols * convLayer->filters->dims + convLayer->layer->layerShape->z;
    PrintCentered("Convolutional Layer");
    printf("Filter Shape: %d x %d x %d\n",convLayer->filterShape->x,convLayer->filterShape->y,convLayer->filterShape->z);
    printf("Output Shape: %d x %d x %d\n",convLayer->layer->layerShape->x,convLayer->layer->layerShape->y,convLayer->layer->layerShape->z);
    printf("Parameters Count: %d\n",convParameters);
    *parametersCount += convParameters;
}