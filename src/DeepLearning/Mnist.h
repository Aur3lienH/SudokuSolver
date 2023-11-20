#pragma once
#include "Matrix.h"
#include "Network.h"
#include "Dataset.h"


void Mnist_Train();

void Mnist_Train_ConvLayers();

void Rotation_Train();

void Mnist_Load(const char* dataFilename);

Dataset* LoadMnist(const char* dataFilename,const char* labelFilename, int magicNumberData, int magicNumberLabel);

Dataset* LoadTestMnist();

Dataset* LoadSudokuDigitDataset();

Dataset* LoadCombinedTrainDataset();

Matrix* S_LabelToMatrix(char label, size_t size);

char S_MatrixToLabel(const Matrix* m);

void Little_Train();

float TestAccuracy(Network* network, Dataset* dataset);

void Reco_Save(Network* network);


Network* LoadBestRecognitionModel();
