#pragma once
#include "Matrix.h"
#include <stdlib.h>

typedef struct
{
    Matrix*** data;
    size_t size;
} Dataset;

Dataset* Dataset_Create(Matrix*** data, size_t size);
void Dataset_Free(Dataset* dataset);