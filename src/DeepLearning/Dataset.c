#include "Dataset.h"
#include <stdlib.h>
#include "Matrix.h"

Dataset* Dataset_Create(Matrix*** data, size_t size)
{
    Dataset* dataset = (Dataset*)malloc(sizeof(Dataset));
    dataset->data = data;
    dataset->size = size;
    return dataset;
}

void Dataset_Free(Dataset* dataset)
{
    for (size_t i = 0; i < dataset->size; i++)
    {
        M_Free(dataset->data[0][i]);
        M_Free(dataset->data[1][i]);
    }
    free(dataset->data[0]);
    free(dataset->data[1]);
    free(dataset->data);
    free(dataset);
}