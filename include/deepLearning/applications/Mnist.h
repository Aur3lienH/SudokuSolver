#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "matrix/Matrix.h"
#include "deepLearning/Network.h"
#include "deepLearning/Dataset.h"







/*

Functionnalities to train on the MNIST dataset and on hand-made dataset 

*/


//Train a network on the mnist dataset with only Feed foward layer and 
void Mnist_Train();

//Train with convlayers
void Mnist_Train_ConvLayers();

//Train a network on Mnist to know in which direction the digit is rotated
void Rotation_Train();

//Load MNIST
void Mnist_Load(const char* dataFilename);

//Load Mnist and checking the magic numbers
Dataset* LoadMnist(const char* dataFilename,const char* labelFilename, int magicNumberData, int magicNumberLabel);

//Load the testing dataset
Dataset* LoadTestMnist();

//Load the hand made training dataset
Dataset* LoadSudokuDigitDataset();

//Combined Mnist and the hand made dataset
Dataset* LoadCombinedTrainDataset();

//Convert a label into a matrix which correspond to the ouptut of the network
Matrix* S_LabelToMatrix(char label, size_t size);

//Convert an output matrix of the network into a label
char S_MatrixToLabel(const Matrix* m);

//Test the accuracy of a network on a given dataset
float TestAccuracy(Network* network, Dataset* dataset);

//Save a mnist model using his accuracy as title
void Reco_Save(Network* network,Dataset* trainSet, Dataset* testSet);

//Load the model with the best test accuracy
Network* LoadBestRecognitionModel();

void SaveDigit(const Matrix* matrix, uint8_t label, FILE* file);

Dataset* LoadHandWritten(const char* path);
