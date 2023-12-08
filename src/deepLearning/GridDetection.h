#pragma once
#include "Dataset.h"
#include "Matrix.h"

void TrainGridRecognition(const char* datasetPath);

Dataset* LoadDataset(const char* path);
