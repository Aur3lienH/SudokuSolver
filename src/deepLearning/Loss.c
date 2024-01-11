#include "deepLearning/Loss.h"
#include "matrix/Matrix.h"
#include <stdlib.h>
#include <math.h>



Loss* MSE_Create()
{
    Loss* loss = (Loss*)malloc(sizeof(Loss));
    loss->Compute = MSE_Compute;
    loss->ComputeDerivative = MSE_Derivative;
    return loss;
}

double MSE_Compute(const Matrix* output, const Matrix* desiredOutput, float* accumulators, float lambda)
{
    double res = 0;
    size_t matrixSize = M_GetSize3D(output);
    for (size_t i = 0; i < matrixSize; i++)
    {
        printf("output %f\n", output->data[i]);
        printf("desiredOutput %f\n", desiredOutput->data[i]);
        res += (desiredOutput->data[i] - output->data[i]) * (desiredOutput->data[i] - output->data[i]);
    }
    res += lambda * *accumulators;
    return res;
}

void MSE_Derivative(const Matrix* output, const Matrix* desiredOutput, Matrix* res)
{
    size_t matrixSize = M_GetSize3D(output);

    for (size_t i = 0; i < matrixSize; i++)
    {
        res->data[i] = output->data[i] - desiredOutput->data[i];
    }

}

Loss* CE_Create()
{
    Loss* loss = (Loss*)malloc(sizeof(Loss));
    loss->Compute = CE_Compute;
    loss->ComputeDerivative = CE_Derivative;
    return loss;
}

double CE_Compute(const Matrix* output, const Matrix* desiredOutput, float* accumulators, float lambda)
{
    double cost = 0;
    size_t matrixSize = M_GetSize3D(output);
    for (size_t i = 0; i < matrixSize; i++)
    {
        cost += desiredOutput->data[i] * log(output->data[i] + 1e-15) + (1 - desiredOutput->data[i]) * log(1 - output->data[i] + 1e-15);
    }
    cost += lambda * *accumulators;
    return -cost / 10;
}

void CE_Derivative(const Matrix* output, const Matrix* desiredOutput, Matrix* res)
{
    size_t matrixSize = M_GetSize3D(output);
    for (size_t i = 0; i < matrixSize; i++)
    {
        if (desiredOutput->data[i] == 1)
        {
            res->data[i] = -1 + output->data[i];
        }
        else
        {
            res->data[i] = output->data[i];
        }
    }
}


