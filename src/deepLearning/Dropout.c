#include "Dropout.h"
#include "Activation.h"
#include "Matrix.h"
#include "LayerShape.h"
#include "Layer.h"
#include "ConsoleTools.h"
#include <stdlib.h>


Layer* Drop_Create(double dropout_rate)
{
    
    Dropout* drop = (Dropout*)malloc(sizeof(Dropout));
    drop->dropout_rate = dropout_rate;
    drop->layer = (Layer*)malloc(sizeof(Layer));
    drop->layer->layerPtr = drop;
    drop->layer->layerType = 2;
    drop->layer->Compile = Drop_Compile;
    drop->layer->FeedForward = Drop_FeedForward;
    drop->layer->Copy = Drop_Copy;
    drop->layer->Save = Drop_Save;
    drop->layer->Load = Drop_Load;
    drop->layer->Free = Drop_Free;
    drop->layer->AddDelta = L_AddDelta;
    drop->layer->AverageDelta = L_AverageDelta;
    drop->layer->Process = L_Process;
    drop->layer->BackPropagation = L_BackPropagate;
    drop->layer->AdjustWeigths = L_Update;
    drop->layer->Print = Drop_Print;



    return drop->layer;
}

void Drop_Compile(void* layerPtr, LayerShape* layerShape)
{
    Dropout* drop = (Dropout*)layerPtr;
    drop->layer->layerShape = LS_Create1D(layerShape->x);
}

Matrix* Drop_FeedForward(void* layerPtr, Matrix* input)
{
    Dropout* drop = (Dropout*)layerPtr;
    size_t size = M_GetSize3D(input);
    for (size_t i = 0; i < size; i++)
    {
        if ((double)rand() / (double)RAND_MAX < drop->dropout_rate)
        {
            input->data[i] = 0;
        }
    }
    drop->layer->outputs = (Matrix *)input;
    return input;
}

Layer* Drop_Copy(void* layerPtr)
{
    Dropout* drop = (Dropout*)layerPtr;
    return Drop_Create(drop->dropout_rate);
}

void Drop_Save(void* layerPtr, FILE* file)
{
    Dropout* drop = (Dropout*)layerPtr;
    fwrite(&drop->dropout_rate, sizeof(double), 1, file);
}

Layer* Drop_Load(FILE* file)
{
    double dropout_rate;
    fread(&dropout_rate, sizeof(double), 1, file);
    return Drop_Create(dropout_rate);
}

void Drop_Free(void* layerPtr)
{
    Dropout* drop = (Dropout*)layerPtr;
    free(drop);
}

void Drop_Print(void* drop)
{
    PrintCentered("Dropout Layer");
    printf("Dropout Rate: %f\n", ((Dropout*)drop)->dropout_rate);
}
