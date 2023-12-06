#include "GridDetection.h"
#include "Dataset.h"
#include "Network.h"
#include "FCL.h"
#include "ConvLayer.h"
#include "MaxPoolLayer.h"
#include "Matrix.h"
#include "Activation.h"
#include "ReshapeLayer.h"
#include "Loss.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


void TrainGridRecognition(const char* datasetPath, const char* networkPath)
{
    Dataset* dataset = LoadDataset(datasetPath);
    if (dataset == NULL)
    {
        return;
    }
    size_t width = dataset->data[0][0]->cols;
    size_t height = dataset->data[0][0]->rows;
    LayerShape* inputShape = LS_Create2D(width, height);
    Network* network = N_CreateNetwork();
    N_AddLayer(network, I_Create_2D(inputShape));
    N_AddLayer(network, Conv_Create(LS_Create3D(3,3,32)));
    N_AddLayer(network, MaxPool_Create(LS_Create2D(2,2)));
    N_AddLayer(network, Conv_Create(LS_Create3D(3,3,64)));
    N_AddLayer(network, MaxPool_Create(LS_Create2D(2,2)));
    N_AddLayer(network, Flatten_Create());
    N_AddLayer(network, FCL_Create(128, ReLU()));
    N_AddLayer(network, FCL_Create(8,     ()));

    N_Compile(network,MSE_Create());

    //Get the thread count by getting the number of cores
    int threadCount = sysconf(_SC_NPROCESSORS_ONLN);

    N_Train(network, dataset, 32, 10,threadCount, 0.01f);

    N_Save(network, networkPath);

    N_Free(network);

}


Dataset* LoadDataset(const char* path)
{
    FILE* file = open(path, "r");
    if (file == NULL)
    {
        printf("Error: Could not open file %s\n", path);
        return NULL;
    }
    int numberOfImages = 0;
    int error = fread(&numberOfImages, sizeof(int), 1, file);
    if (error != 1)
    {
        printf("Error: Could not read number of images from file %s\n", path);
        return NULL;
    }
    int width = 0;
    error = fread(&width, sizeof(int), 1, file);
    if (error != 1)
    {
        printf("Error: Could not read width from file %s\n", path);
        return NULL;
    }
    Dataset* dataset = malloc(sizeof(Dataset));
    dataset->size = numberOfImages;
    dataset->data = malloc(sizeof(Matrix**) * 2);
    dataset->data[0] = malloc(sizeof(Matrix*) * numberOfImages);
    dataset->data[1] = malloc(sizeof(Matrix*) * numberOfImages);

    for (size_t i = 0; i < numberOfImages; i++)
    {
        dataset->data[0][i] = M_Load(file);
        dataset->data[1][i] = M_Load(file);
    }

    fclose(file);
    return dataset;
}