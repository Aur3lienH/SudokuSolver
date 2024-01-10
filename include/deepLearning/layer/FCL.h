#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "../Network.h"
#include "../Adam.h"
#include "../LayerShape.h"
#include "../Activation.h"

/*

FCL : Fully Connected Layer 
Classic perceptron layer

*/



struct Size
{
    size_t rows;
    size_t cols;
    size_t dims;
};

typedef struct
{
    Layer* layer;
    Matrix* weights;
    Matrix* biases;
    Matrix* delta;
    Matrix* tempDelta;
    Matrix* deltaBiases;
    Matrix* z;
    Matrix* newDelta;
    Matrix* deltaActivation;
    Activation* activation;
    Adam* adamWeigths;
    Adam* adamBiases;
    unsigned int NeuronsCount;
    void (*M_Mul)(const Matrix* a,const Matrix* b, Matrix* c);
    void (*M_Transpose1Mul)(const Matrix* a,const Matrix* b, Matrix* c);
    void (*M_Transpose2Mul)(const Matrix* a,const Matrix* b, Matrix* c);
} FCL;


/// @brief Create a Fully Connected Layer
/// @param neuronsCount The number of neurons in the layer
/// @param Activation The activation function applied to the layer
/// @return Returns the layer created to be used in the network
Layer* FCL_Create(unsigned int neuronsCount, Activation* Activation);

/// @brief Free the layer
void FCL_Free(void* layerPtr);

/// @brief Compile the network, this function is called by the network itself
/// @param layer The layer to compile
/// @param layerShape The shape of the previous layer
void FCL_Compile(void* layer,LayerShape* layerShape);

/// @brief Process the input threw the layer and return the output
/// @param layerPtr The layer 
/// @param input The input which will be process
/// @return The result of the calculation
Matrix* FCL_FeedForward(void* layerPtr, Matrix* input);

/// @brief Process the input threw the layer and return the output
/// @param layerPtr The layer
/// @param input The input which will be process
/// @return The result of the calculation
Matrix* FCL_Process(void* layerPtr, Matrix* input);

/// @brief Calculate the gradient of the layer depending on the previous Activation of the layer
/// @param layerPtr The layer
/// @param input The output of the previous layer
/// @param delta The delta of the next layer, or the derivative of the cost function if it is the last layer
/// @return The gradient depending on the previous layer
Matrix* FCL_Backpropagation(void* layerPtr, Matrix* input, Matrix* delta);

/// @brief Update the weights of the layer
void FCL_UpdateWeights(void* layerPtr,float learningRate, float* accumulator, float lambda);

/// @brief Save the layer
/// @param layerPtr The layer
/// @param file The file to save the layer
void FCL_Save(void* layerPtr, FILE* file);

/// @brief Load the layer
/// @param file The file to load the layer
/// @return The layer loaded
Layer* FCL_Load(FILE* file);

/// @brief Copy the layer
/// @param layerPtr The layer
/// @return The copy of the layer
Layer* FCL_Copy(void* layerPtr);


/// @brief Add the delta of the layer to the layer
/// @param layerPtr The layer
/// @param layer2Ptr The layer to add
void FCL_AddDelta(void* layerPtr, void* layer2Ptr);

/// @brief Average the delta of the layer
/// @param layerPtr The layer
/// @param size The size of the dataset
void FCL_AverageDelta(void* layerPtr, const size_t size);


/// @brief Print the specififcations of the layer
/// @param layerPtr The layer
void FCL_Print(void* layerPtr, int* parametersCount);




