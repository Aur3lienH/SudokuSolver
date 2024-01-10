#pragma once
#include "../Dataset.h"
#include "matrix/Matrix.h"


/*

Main purpose : Train a network to get the four corners of a sudoku grid

*/



//Train the network
void TrainGridRecognition(const char* datasetPath);

//Load the dataset for the training
Dataset* LoadDataset(const char* path);
