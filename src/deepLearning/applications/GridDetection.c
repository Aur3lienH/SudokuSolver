#include "deepLearning/applications/GridDetection.h"
#include "deepLearning/Dataset.h"
#include "deepLearning/Network.h"
#include "deepLearning/layer/FCL.h"
#include "deepLearning/layer/ConvLayer.h"
#include "deepLearning/layer/MaxPoolLayer.h"
#include "matrix/Matrix.h"
#include "deepLearning/Activation.h"
#include "deepLearning/layer/ReshapeLayer.h"
#include "deepLearning/Loss.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


void TrainGridRecognition(const char* datasetPath)
{
    Dataset* dataset = LoadDataset(datasetPath);
    printf("Dataset loaded\n");
    if (dataset == NULL)
    {
        return;
    }


    size_t width = dataset->data[0][0]->cols;
    size_t height = dataset->data[0][0]->rows;
    printf("Width: %ld\n", width);
    printf("Height: %ld\n", height);
    printf("dataset size : %ld\n", dataset->size);
    LayerShape* inputShape = LS_Create2D(width, height);
    Network* network = N_CreateNetwork();
    N_AddLayer(network, I_Create_2D(inputShape));
    N_AddLayer(network, Conv_Create(LS_Create3D(3,3,32)));
    N_AddLayer(network, MaxPool_Create(2));
    N_AddLayer(network, Conv_Create(LS_Create3D(3,3,16)));
    N_AddLayer(network, MaxPool_Create(13));
    N_AddLayer(network, Flatten_Create());
    N_AddLayer(network, FCL_Create(128, ReLU()));
    N_AddLayer(network, FCL_Create(8, Linear()));
    printf("Network created\n");
    N_Compile(network,MSE_Create());
    N_Print(network);
    printf("Network compiled\n");

    //Get the thread count by getting the number of cores
    int threadCount = sysconf(_SC_NPROCESSORS_ONLN);

    N_Train(network, dataset, 1, 10,1, 0.01f);

    N_Save(network, "./models/grid/kadir.model");

    N_Free(network);

}


Dataset* LoadDataset(const char* path)
{
    FILE* file = fopen(path, "rb");
    if (file == NULL)
    {
        printf("Error: Could not open file %s\n", path);
        return NULL;
    }
    printf("Loading dataset from %s\n", path);
    int numberOfImages = 0;
    int error = fread(&numberOfImages, sizeof(int), 1, file);
    if (error != 1)
    {
        printf("Error: Could not read number of images from file %s\n", path);
        return NULL;
    }
    printf("Number of images: %d\n", numberOfImages);
    int width = 0;
    error = fread(&width, sizeof(int), 1, file);
    if (error != 1)
    {
        printf("Error: Could not read width from file %s\n", path);
        return NULL;
    }
    printf("Width: %d\n", width);
    Dataset* dataset = malloc(sizeof(Dataset));
    numberOfImages -= 1;
    dataset->size = numberOfImages;
    dataset->data = malloc(sizeof(Matrix**) * 2);
    dataset->data[0] = malloc(sizeof(Matrix*) * numberOfImages);
    dataset->data[1] = malloc(sizeof(Matrix*) * numberOfImages);

    for (size_t i = 0; i < numberOfImages; i++)
    {
        printf("Loading image %ld\n", i);
        dataset->data[0][i] = M_Load(file);
        dataset->data[1][i] = M_Load(file);
        M_Dim(dataset->data[0][i]);
        M_Dim(dataset->data[1][i]);
        printf("Loading image %ld\n", i);
    }

    fclose(file);
    return dataset;
}