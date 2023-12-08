#include "FCL.h"
#include "Adam.h"
#include "Matrix.h"
#include "BlockMatrix.h"
#include "LayerShape.h"
#include "Network.h"
#include "Activation.h"
#include "RandomInit.h"
#include "ConsoleTools.h"
#include <stdio.h>
#include <err.h>
#include <stdlib.h>



Layer* FCL_Create(unsigned int neuronsCount, Activation* activation)
{
    FCL* fcl = (FCL*)malloc(sizeof(FCL));
    fcl->NeuronsCount = neuronsCount;
    fcl->activation = activation;
    fcl->weights = NULL;
    fcl->biases = NULL;

    if(M_GetMatrixType() == 0)
    {
        fcl->M_Mul = M_Mul;
        fcl->M_Transpose1Mul = M_Transpose1Mul;
        fcl->M_Transpose2Mul = M_Transpose2Mul;
    }
    else
    {
        fcl->M_Mul = BM_M_SSE_Mul;
        fcl->M_Transpose1Mul = BM_M_AVX_Transpose1Mul;
        fcl->M_Transpose2Mul = BM_M_SSE_Transpose2Mul;
    }


    fcl->layer = (Layer*)malloc(sizeof(Layer));
    fcl->layer->Load = FCL_Load;
    fcl->layer->Save = FCL_Save;
    fcl->layer->FeedForward = FCL_FeedForward;
    fcl->layer->Process = FCL_Process;
    fcl->layer->BackPropagation = FCL_Backpropagation;
    fcl->layer->AdjustWeigths = FCL_UpdateWeights;
    fcl->layer->Copy = FCL_Copy;
    fcl->layer->outputs = M_Create_2D(fcl->NeuronsCount,1);
    fcl->layer->Compile = FCL_Compile;
    fcl->layer->layerShape = LS_Create1D(neuronsCount);
    fcl->layer->layerPtr = fcl;
    fcl->layer->Free = FCL_Free;
    fcl->layer->layerType = 1;
    fcl->layer->AddDelta = FCL_AddDelta;
    fcl->layer->AverageDelta = FCL_AverageDelta;
    fcl->layer->Print = FCL_Print;
    return fcl->layer;
}

void FCL_Free(void* layerPtr)
{
    FCL* fcl = (FCL*)layerPtr;
    M_Free(fcl->weights);
    M_Free(fcl->biases);
    M_Free(fcl->delta);
    M_Free(fcl->deltaBiases);
    M_Free(fcl->z);
    M_Free(fcl->newDelta);
    M_Free(fcl->tempDelta);
    M_Free(fcl->deltaActivation);
    L_Free(fcl->layer);
    Adam_Free(fcl->adamBiases);
    Adam_Free(fcl->adamWeigths);
    free(fcl->activation);
    free(fcl);
}

/// @brief Compile the layer
/// @param fcl 
/// @param previousNeuronsCount PreviousNeuronsCount used to compute weigths
void FCL_Compile(void* layerPtr,LayerShape* previousLayerShape)
{
    FCL* fcl = (FCL*)layerPtr;
    //Init the weigths and his delta
    if(fcl->weights == NULL)
    {
        fcl->weights = SM_Create_2D(fcl->NeuronsCount,previousLayerShape->x);
        HeUniform(fcl->weights);
    }

    
    fcl->delta = SM_Create_2D(fcl->NeuronsCount,previousLayerShape->x);
    fcl->tempDelta = SM_Create_2D(fcl->NeuronsCount,previousLayerShape->x);


    //Init the biases and his delta
    if(fcl->biases == NULL)
    {
        fcl->biases = M_Create_2D(fcl->NeuronsCount,1);
    }
    fcl->deltaBiases = M_Create_2D(fcl->NeuronsCount,1);
    
    fcl->deltaActivation = M_Create_2D(fcl->NeuronsCount,1);
    fcl->newDelta = M_Create_2D(previousLayerShape->x,1);

    fcl->z = M_Create_2D(fcl->NeuronsCount,1);

    fcl->adamBiases = Adam_Create(0.9,0.999,1e-7,fcl->biases->rows * fcl->biases->cols);
    fcl->adamWeigths = Adam_Create(0.9,0.999,1e-7,fcl->weights->rows * fcl->weights->cols);
    
}

Matrix* FCL_FeedForward(void* layerPtr,Matrix* input)
{
    FCL* fcl = (FCL*)layerPtr;

    //output = input * weights + biases
    fcl->M_Mul(fcl->weights, input, fcl->z);
    M_Add(fcl->z, fcl->biases, fcl->z);


    //output = activation(output)
    fcl->activation->Compute(fcl->z, fcl->layer->outputs);

    return fcl->layer->outputs;
}

Matrix* FCL_Process(void* layerPtr, Matrix* input)
{
    return FCL_FeedForward(layerPtr,input);
}


/// @brief Calculate gradient and store it in delta and deltaBiases
/// @param fcl Is the Fully Connected Layer
/// @param input Is the input of the layer in the feed forward pass, so the output of the previous layer
/// @param delta is the delta of the next layer, calculated just before
/// @return It returns the delta for the previous layer
Matrix* FCL_Backpropagation(void* layerPtr, Matrix* input, Matrix* delta)
{
    
    FCL* fcl = (FCL*)layerPtr;
    fcl->activation->ComputeDerivative(fcl->z, fcl->deltaActivation);
    M_LinearMul(fcl->deltaActivation, delta, fcl->deltaActivation);

    M_Add(fcl->deltaActivation,fcl->deltaBiases,fcl->deltaBiases);

    fcl->M_Transpose2Mul(fcl->deltaActivation,input,fcl->delta);

    
    
    fcl->M_Transpose1Mul(fcl->weights,fcl->deltaActivation,fcl->newDelta);

    return fcl->newDelta;
}


/// @brief Update the weigths with the calculated gradients
/// @param fcl Is the fully Connected Layer
/// @param learningRate the learning rate
void FCL_UpdateWeights(void* layerPtr,float learningRate,float* accumulator, float lambda)
{
    FCL* fcl = (FCL*)layerPtr;
    Adam_Update_L2(fcl->adamWeigths, fcl->weights->data, fcl->delta->data, learningRate, accumulator, lambda);
    Adam_Update(fcl->adamBiases,fcl->biases->data,fcl->deltaBiases->data,learningRate);
    //M_Print(fcl->weights);

    M_Zero(fcl->delta);
    M_Zero(fcl->deltaBiases);

}

void FCL_Save(void* layer, FILE* file)
{
    FCL* fcl = (FCL*)layer;
    fwrite(&fcl->NeuronsCount,sizeof(unsigned int),1,file);
    fwrite(&fcl->activation->activationType,sizeof(unsigned char),1,file);
    
    M_Save(fcl->weights,file);
    M_Save(fcl->biases,file);
}

Layer* FCL_Load(FILE* file)
{
    unsigned int neuronsCount;
    Activation* activation = NULL;
    fread(&neuronsCount,sizeof(unsigned int),1,file);
    unsigned char activationType;
    fread(&activationType,sizeof(unsigned char),1,file);

    switch (activationType)
    {
    case 0:
        activation = Sigmoid();
        break;
    case 1:
        activation = ReLU();
        break;
    case 2:
        activation = Softmax();
        break;
    }

    Layer* layer = FCL_Create(neuronsCount,activation);
    FCL* fcl = (FCL*)layer->layerPtr;
    fcl->weights = M_Load(file);
    fcl->biases = M_Load(file);

    return layer;
}

Layer* FCL_Copy(void* layerPtr)
{
    FCL* fcl = (FCL*)layerPtr;
    Layer* layer = FCL_Create(fcl->NeuronsCount,fcl->activation);
    FCL* newFcl = (FCL*)layer->layerPtr;
    newFcl->weights = fcl->weights;
    newFcl->biases = fcl->biases;
    return layer;
}


void FCL_AddDelta(void* layerPtr, void* layer2Ptr)
{
    FCL* fcl = (FCL*)layerPtr;
    FCL* fcl2 = (FCL*)layer2Ptr;
    M_Add(fcl->delta,fcl2->delta,fcl->delta);
    M_Add(fcl->deltaBiases,fcl2->deltaBiases,fcl->deltaBiases);
    M_Zero(fcl2->delta);
    M_Zero(fcl2->deltaBiases);
    
}

void FCL_AverageDelta(void* layerPtr, const size_t size)
{
    FCL* fcl = (FCL*)layerPtr;
    M_ScalarMul(fcl->delta,1.0f / size,fcl->delta);
    M_ScalarMul(fcl->deltaBiases,1.0f / size,fcl->deltaBiases);
}


void FCL_Print(void* layerPtr, int* parametersCount)
{
    FCL* fcl = (FCL*)layerPtr;
    size_t fclParametersCount = M_GetSize3D(fcl->weights) + M_GetSize3D(fcl->biases);
    PrintCentered("Fully Connected Layer");
    printf("Neurons Count : %d\n",fcl->NeuronsCount);
    printf("Activation : ");
    switch (fcl->activation->activationType)
    {
    case 0:
        printf("Sigmoid\n");
        break;
    case 1:
        printf("ReLU\n");
        break;
    case 2:
        printf("Softmax\n");
        break;
    }

    printf("Parameters Count : %zu\n",fclParametersCount);
    *parametersCount += fclParametersCount;
}

