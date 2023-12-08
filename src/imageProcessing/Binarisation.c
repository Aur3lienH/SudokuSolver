#include "Binarisation.h"
#include <stdlib.h>
#include "../deepLearning/Matrix.h"



Matrix* Binarisation(Matrix* input, float threshold)
{
    Matrix* output = M_Create_2D(input->cols, input->rows);
    for (size_t i = 0; i < input->rows; i++)
    {
        for (size_t j = 0; j < input->cols; j++)
        {
            if (input->data[j + i * input->cols] > threshold)
            {
                output->data[j + i * input->cols] = 1;
            }
            else
            {
                output->data[j + i * input->cols] = 0;
            }
        }
    }
    return output;
}


float M_Mean(Matrix* input)
{
    float mean = 0;
    for (size_t i = 0; i < input->rows; i++)
    {
        for (size_t j = 0; j < input->cols; j++)
        {
            mean += input->data[j + i * input->cols];
        }
    }
    return mean / (input->rows * input->cols);
}

float M_Variance(Matrix* input)
{
    float mean = M_Mean(input);
    float variance = 0;
    for (size_t i = 0; i < input->rows; i++)
    {
        for (size_t j = 0; j < input->cols; j++)
        {
            variance += (input->data[j + i * input->cols] - mean) * (input->data[j + i * input->cols] - mean);
        }
    }
    return variance / (input->rows * input->cols);
}

void M_Inverse(Matrix* input)
{
    for (size_t i = 0; i < input->rows; i++)
    {
        for (size_t j = 0; j < input->cols; j++)
        {
            input->data[j + i * input->cols] = 1 - input->data[j + i * input->cols];
        }
    }
}

Matrix* M_OptimalBinarisation(Matrix* input)
{
    float threshold = 0;
    float maxVariance = 0;
    for (size_t i = 0; i < 255; i++)
    {
        Matrix* binarised = Binarisation(input, i/255.0f);
        float variance = M_Variance(binarised);
        if (variance > maxVariance)
        {
            maxVariance = variance;
            threshold = i;
        }
        M_Free(binarised);
    }
    printf("Threshold : %f\n", threshold);
    Matrix* binarised = Binarisation(input, 0.75f);
    M_Inverse(binarised);
    return binarised;
}

