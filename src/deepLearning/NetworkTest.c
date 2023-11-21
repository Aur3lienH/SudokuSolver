#include "NetworkTest.h"
#include "Network.h"
#include "ConvLayer.h"
#include "FCL.h"
#include "Loss.h"
#include "Matrix.h"
#include "LayerShape.h"
#include "InputLayer.h"
#include "Dataset.h"
#include "MatrixTests.h"

#include <math.h>


int ConvLayerBackpropagationTest(char** name)
{
    *name = "ConvLayerBackpropagationTest";
    
    Layer* convLayer = Conv_Create(LS_Create3D(2,2,1));
    convLayer->layerPos = 1;
    convLayer->Compile(convLayer->layerPtr,LS_Create3D(3,3,1));
    ConvLayer* convLayerPtr = (ConvLayer*)convLayer->layerPtr;
    convLayerPtr->filters->data[0] = 1;
    convLayerPtr->filters->data[1] = 2;
    convLayerPtr->filters->data[2] = 3;
    convLayerPtr->filters->data[3] = 4;

    
    Matrix* input = M_Create_3D(3,3,1);
    input->data[0] = 1;
    input->data[1] = 2;
    input->data[2] = 3;

    input->data[3] = 4;
    input->data[4] = 5;
    input->data[5] = 5;

    input->data[6] = 7;
    input->data[7] = -3;
    input->data[8] = -2;
    
    Matrix* previousDelta = M_Create_3D(2,2,1);
    previousDelta->data[0] = 2;
    previousDelta->data[1] = 3;
    previousDelta->data[2] = 1;
    previousDelta->data[3] = -1;
    Matrix* res = convLayer->FeedForward(convLayer->layerPtr,input);
    M_Print(res, "res of conv");
    M_Print(convLayerPtr->deltaActivation, "delta activation before backprop");
    Matrix* delta = convLayer->BackPropagation(convLayer->layerPtr,input,previousDelta);  
    M_Print(convLayerPtr->deltaActivation, "delta activation after backprop");
    M_Print(convLayerPtr->delta, "calculated delta");
    M_Print(previousDelta, "previous delta");
    M_Free(input);
    M_Free(previousDelta);
    M_Free(res);
    M_Free(delta);
    return 0;
}


void NetworkTest()
{
    TestFunction(ConvLayerBackpropagationTest);
}