#pragma once
#include <pthread.h>
#include "matrix/Matrix.h"
#include "Adam.h"
#include "Loss.h"
#include "LayerShape.h"
#include "Dataset.h"
#include "layer/InputLayer.h"
#include "Network.h"
#include "layer/Layer.h"
#include "Barrier.h"
#include "matrix/Matrix.h"


/*

Neural network structure
The neural network is composed with layers 
the first layer is ALWAYS an input layer
it has a loss function to compute the error of the network

*/



typedef Matrix* (*BackpropFunc)(void* layer, const Matrix* input, const Matrix* delta);
typedef Matrix* (*FeedForwardFunc)(void* layer, const Matrix* input);
typedef void (*UpdateFunc)(void* layer,const float learningRate, float* accumulator, float lambda);
typedef void (*AddDelta)(void* layer, void* delta);
typedef void (*AverageDelta)(void* layer, const size_t size);


typedef struct
{
    Adam* adam;
    Layer** layers;
    Loss* loss;
    size_t layersCount;
    Matrix* lossDerivative;
    //The sum of the weights squared for the regularization
    float* accumulator;
    float lambda;
} Network;



typedef struct 
{
    Network* n;
    Dataset* dataset;
    size_t batchSize;
    size_t epochsCount;
    pthread_mutex_t* mutex;
    pthread_barrier_t* barrier;
} ThreadArgs;






/// @brief Create a neural network
Network* N_CreateNetwork();

//Free the network
void N_Free(Network* n);

/// @brief Compile the network (necessary before using the network)
void N_Compile_L2(Network* n, Loss* loss, float lambda);

void N_Compile(Network* n, Loss* loss);

//Feed forward the input through the network and put the output in the output parameter
float N_FeedForward(Network* n, Matrix* input, Matrix* desiredOutput);

/// @brief Process the input through the network and return the result
/// @param n a network
/// @return the output of the network
Matrix* N_Process(Network* n, Matrix* input);


//Back propagate the error through the network and put the output in the output parameter
void N_BackPropagate(Network* n, Matrix* delta);


//Back propagate the error through the network and return the error of the network
double N_Backpropagation(Network* n, Matrix* input, Matrix* desiredOutput);


void* N_BackpropagationThread(void* args);

/// @brief 
/// @param n the network
/// @param dataset Points to two array : the input data array and desired output array
void N_Train(Network* n, Dataset* dataset,size_t testSetSize, size_t batchSize, size_t epochsCount, size_t threadsCount, float learningRate);

//Update weigths and biases
void N_Update(Network* n, float learningRate);

/// @brief Add a layer to the network
/// @param n a network
/// @param layer the new layer
void N_AddLayer(Network* n, Layer* layer);

/// @brief Save a network
/// @param n the network
/// @param path the path of the network
void N_Save(Network* n, const char* path);

/// @brief Load a network
/// @param path the path of the network
/// @return the network loaded
Network* N_Load(const char* path);

void N_AddDelta(Network* n, Network* n2);

void N_AverageDelta(Network* n, const size_t size);

void N_ShuffleDataset(Dataset* dataset);

void N_Print(Network* n);

void N_Reset(Network* n);
