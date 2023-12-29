#pragma once
#include "../Activation.h"
#include "../Layer.h"


typedef struct Conv_Depth
{
    Layer* layer;
    unsigned int outputDepth;
    LayerShape* filterShape;
    Matrix* filterDepthWise;
    Matrix* filterPointWise;
    Matrix* biases;
    void* (*ConvDepthWise)(const Matrix* input, const Matrix* filter, Matrix* output);
    void (*ConvBackward)(const Matrix* input, const Matrix* filter, Matrix* output);

} Conv_Depth;


/// @brief Create a 
/// @param filterShape the shape of the filter
/// @param Activation 
/// @return Returns the layer created to be used in the network
Layer* Conv_Depth_Create(LayerShape* filterShape, unsigned int outputDepth);

/// @brief Free the layer
void Conv_Depth_Free(void* layerPtr);

/// @brief Compile the layer, this function is called by the network itself
/// @param layer The layer to compile
/// @param layerShape The shape of the previous layer
void Conv_Depth_Compile(void* layer,LayerShape* layerShape);

/// @brief Process the input threw the layer and return the output
/// @param layerPtr The layer 
/// @param input The input which will be process
/// @return The result of the calculation
Matrix* Conv_Depth_FeedForward(void* layerPtr, Matrix* input);

/// @brief Process the input threw the layer and return the output
/// @param layerPtr The layer
/// @param input The input which will be process
/// @return The result of the calculation
Matrix* Conv_Depth_Process(void* layerPtr, Matrix* input);

/// @brief Calculate the gradient of the layer depending on the previous Activation of the layer
/// @param layerPtr The layer
/// @param input The output of the previous layer
/// @param delta The delta of the next layer, or the derivative of the cost function if it is the last layer
/// @return The gradient depending on the previous layer
Matrix* Conv_Depth_Backpropagation(void* layerPtr, Matrix* input, Matrix* delta);

/// @brief Update the weights of the layer
void Conv_Depth_UpdateWeights(void* layerPtr,float learningRate, float* accumulator, float lambda);

/// @brief Save the layer
/// @param layerPtr The layer
/// @param file The file to save the layer
void Conv_Depth_Save(void* layerPtr, FILE* file);

/// @brief Load the layer
/// @param file The file to load the layer
/// @return The layer loaded
Layer* Conv_Depth_Load(FILE* file);

/// @brief Copy the layer
/// @param layerPtr The layer
/// @return The copy of the layer
Layer* Conv_Depth_Copy(void* layerPtr);


/// @brief Add the delta of the layer to the layer
/// @param layerPtr The layer
/// @param layer2Ptr The layer to add
void Conv_Depth_AddDelta(void* layerPtr, void* layer2Ptr);

/// @brief Average the delta of the layer
/// @param layerPtr The layer
/// @param size The size of the dataset
void Conv_Depth_AverageDelta(void* layerPtr, const size_t size);


/// @brief Print the specififcations of the layer
/// @param layerPtr The layer
void Conv_Depth_Print(void* layerPtr, int* parametersCount);
