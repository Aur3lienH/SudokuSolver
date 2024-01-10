#include "deepLearning/RandomInit.h"
#include "matrix/Matrix.h"
#include "matrix/BlockMatrix.h"
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <err.h>


const float noise = 0.1f;


void HeUniform(Matrix* m)
{
    float std = sqrtf(2.0f / m->cols);
    for (size_t i = 0; i < m->effectiveCols; i++)
    {
        for (size_t j = 0; j < m->effectiveRows; j++)
        {
            SM_SetValue(m,j + i * m->effectiveRows,(float)rand() / RAND_MAX * 2 * std - std);
        }
    }

    
}

void M_HeUniform(Matrix* m)
{
    float std = sqrtf(2.0f / m->cols);
    size_t count = m->cols * m->rows * m->dims;
    for (size_t i = 0; i < count; i++)
    {
        m->data[i] = (float)rand() / RAND_MAX * 2 * std - std;
    }
}

void M_HeUniformConv(Matrix* m, size_t previousLayerdDim)
{
    float std = sqrtf(2.0f / (m->cols * m->rows * previousLayerdDim));
    size_t count = m->cols * m->rows * m->dims;
    for (size_t i = 0; i < count; i++)
    {
        m->data[i] = (float)rand() / RAND_MAX * 2 * std - std;
    }
}

void M_AddNoise(const Matrix* m, Matrix* output)
{
    for (size_t i = 0; i < m->cols; i++)
    {
        for (size_t j = 0; j < m->rows; j++)
        {
            //printf("noise : %f\n",((float)rand() / RAND_MAX * noise - noise / 2));

            double u, v;

            // Get two uniform random variables to convert to a Gaussian distribution
            do {
                u = (double)rand() / RAND_MAX;
                v = (double)rand() / RAND_MAX;
            } while (u <= 1e-10); // avoid 0

            // Perform Box-Muller transform
            double num = sqrt(-2.0 * log(u)) * cos(2.0 * 3.14 * v);

            // Scale by standard deviation and shift by mean
            num = num * noise;

            output->data[j + i * m->rows] = m->data[j + i * m->rows] + num;
        }
    }
}

