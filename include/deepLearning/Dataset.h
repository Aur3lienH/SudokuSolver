#pragma once
#include "matrix/Matrix.h"
#include <stdlib.h>

/*
Dataset structure
data : two dimensional array of Matrix : 
    - first element is the input : dataset[0][i]
    - second element is the expected output : dataset[1][i]
*/



typedef struct
{
    Matrix*** data;
    size_t size;
} Dataset;

Dataset* Dataset_Create(Matrix*** data, size_t size);
void Dataset_Free(Dataset* dataset);