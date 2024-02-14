#include "matrix/Matrix.h"
#include "matrix/BlockMatrix.h"
#include "deepLearning/Allocator.h"
#include "tools/FileTools.h"
#include <stdlib.h>
#include <math.h>
#include <err.h>
#include <float.h>
#include <limits.h>

#ifdef __arm__
    #define SSE 0
#elif defined(__aarch64__)
    #define SSE 0
#else
    #define SSE 1
    #define AVX 1
    #include <immintrin.h>
    #include <emmintrin.h>
#endif

#define DEBUG 0

const int matrixType = 1;

Matrix* SM_Create_3D(size_t rows, size_t cols, size_t dims)
{
    if(matrixType == 0)
    {
        return M_Create_3D(rows,cols,dims);
    }
    else
    {
        return BM_Create_3D(rows,cols,dims);
    }
}

Matrix* SM_Create_2D(size_t rows, size_t cols)
{
    return SM_Create_3D(rows,cols,1);
}

Matrix* M_Create_3D(size_t rows, size_t cols, size_t dims)
{
    return M_Init(rows,cols,dims,rows,cols);
}

Matrix* M_Create_3D_Data(size_t rows, size_t cols, size_t dims, float* data)
{
    Matrix* m = (Matrix*)malloc(sizeof(Matrix));
    m->data = data;
    m->rows = rows;
    m->cols = cols;
    m->dims = dims;
    m->effectiveRows = rows;
    m->effectiveCols = cols;
    return m;
}

Matrix* M_Create_2D_Data(size_t rows, size_t cols, float* data)
{
    return M_Create_3D_Data(rows,cols,1,data);
}

size_t M_ConvertIndex(const Matrix* m, size_t rows, size_t cols)
{
    return (rows * m->cols + cols);
}

Matrix* M_Init(size_t rows, size_t cols, size_t dims, size_t effectiveRows, size_t effectiveCols)
{
    Matrix* m = (Matrix*)malloc(sizeof(Matrix));
#if SSE
    m->data = (float*)_mm_malloc(rows * cols * dims * sizeof(float),64);
    if(m->data == NULL)
    {
        errx(-1,"M_Init(): Could not allocate memory\n");
    }
#else
    m->data = (float*)malloc(rows * cols * dims * sizeof(float));
#endif
    m->rows = rows;
    m->cols = cols;
    m->dims = dims;
    m->effectiveRows = effectiveRows;
    m->effectiveCols = effectiveCols;
    for (size_t i = 0; i < rows * cols * dims; i++)
    {
        m->data[i] = 0;
    }
    
    return m;
}


Matrix* M_Create_2D(size_t rows, size_t cols)
{
    return M_Create_3D(rows,cols,1);
}

Matrix* M_LS_To_Matrix(const LayerShape* ls)
{
    return M_Create_3D(ls->x,ls->y,ls->z);
}

void M_Free(Matrix* m)
{
#if SSE
    _mm_free(m->data);
#else
    free(m->data);
#endif
    free(m);
}


void M_Print(const Matrix* m, const char* name)
{
    
    
    printf("%s : %li x %li x %li\n",name,m->rows,m->cols,m->dims);
    float* dataPtr = m->data;
    for (size_t k = 0; k < m->dims; k++)
    {
        for (size_t i = 0; i < m->rows; i++)
        {
            printf("[");
            for (size_t j = 0; j < m->cols; j++)
            {
                printf("%f",dataPtr[i * m->cols + j]);
                if (j != m->cols - 1)
                {
                    printf(" ");
                }
            }
            printf("]\n");
        }
        if(k != m->dims - 1)
        {
            printf("--------------");
            
            printf("\n");
        }
        dataPtr += M_GetSize2D(m);
        
    }
}


void M_Print2D(const Matrix* input,const char* name)
{
    printf("%s : %li x %li\n",name,input->rows,input->cols);
    for (size_t i = 0; i < input->rows; i++)
    {
        printf("[");
        for (size_t j = 0; j < input->cols; j++)
        {
            printf("%f",input->data[i * input->cols + j]);
            if (j != input->cols - 1)
            {
                printf(" ");
            }
        }
        printf("]\n");
    }
}

void M_Dim(const Matrix* m)
{
    printf("Matrix: %li x %li x %li\n",m->rows,m->cols,m->dims); 
}


Matrix* M_FillMatrixZero(const Matrix* input,size_t rowsToFill,size_t colsToFill)
{

    Matrix* res = M_Create_2D(input->rows + rowsToFill * 2, input->cols + colsToFill * 2);
    for (size_t i = 0; i < input->rows; i++)
    {   
        for (size_t j = 0; j < input->cols; j++)
        {
            res->data[(i + rowsToFill) * res->cols + (j + colsToFill)] = input->data[i * input->cols + j];
        }
    }
    return res;
}

 
//Multily the first two matrices and put the ouptut in the third parameter
void M_Mul(const Matrix* a,const Matrix* b, Matrix* output)
{
#if DEBUG
    if(a->cols != b->rows || a->rows != output->rows || b->cols != output->cols)
    {
        errx(-1,"M_Mul(): Matrix dimensions do not match\n");
        return;
    }
#endif


    for (size_t i = 0; i < a->rows; i++)
    {
        for (size_t j = 0; j < b->cols; j++)
        {
            output->data[i * b->cols + j] = 0;
            for (size_t k = 0; k < a->cols; k++)
            {
                output->data[i * output->cols + j] += a->data[i * a->cols + k] * b->data[k * b->cols + j];
            }
        }
    }
}

//Multiplay all the elements of the matrix by a scalar
void M_ScalarMul(const Matrix* m,const float scalar, Matrix* output)
{
    size_t matrixSize = M_GetSize3D(m);
    for (size_t i = 0; i < matrixSize; i++)
    {
        output->data[i] = m->data[i] * scalar;
    }
}

//Add the first two matrices and put the ouptut in the third parameter
void M_Add(const Matrix* a,const Matrix* b, Matrix* output)
{

#if DEBUG

    if(a->rows != b->rows || a->cols != b->cols || a->rows != output->rows || a->cols != output->cols)
    {
        errx(-1,"M_Add(): Matrix dimensions do not match\n");
    }

#endif


    size_t matrixSize = M_GetSize3D(a);
    for (size_t i = 0; i < matrixSize; i++)
    {
        output->data[i] = a->data[i] + b->data[i];
    }
}

void M_AddScalar(const Matrix* m, const float scalar, Matrix* output)
{
    size_t matrixSize = M_GetSize3D(m);
    for (size_t i = 0; i < matrixSize; i++)
    {
        output->data[i] = m->data[i] + scalar;
    }
}


float M_GetMax(const Matrix* m)
{
    float max = FLT_MIN;
    size_t matrixSize = M_GetSize3D(m);
    for (size_t i = 0; i < matrixSize; i++)
    {
        if(m->data[i] > max)
        {
            max = m->data[i];
        }
    }
    return max;
}


float M_Get(const Matrix* m, size_t rows, size_t cols)
{
    return m->data[cols + rows * m->cols];
}


//Apply function to all the elements of the matrix
void M_Apply(float (*func)(float),const Matrix* m, Matrix* output)
{
    size_t matrixSize = M_GetSize3D(m);
    for (size_t i = 0; i < matrixSize; i++)
    {
        output->data[i] = func(m->data[i]);
    }
}


//Multiplay all the elements of the matrix by all the elements of the second matrix and put the ouptut in the third parameter
void M_LinearMul(const Matrix* a,const Matrix* b, Matrix* output)
{
#if DEBUG
    if(a->rows != b->rows || a->cols != b->cols || a->rows != output->rows || a->cols != output->cols)
    {
        M_Dim(a);
        M_Dim(b);
        M_Dim(output);
        errx(-1,"M_LinearMul(): Matrix dimensions do not match\n");
        return;
    }
#endif

    size_t matrixSize = M_GetSize3D(a);
    for (size_t i = 0; i < matrixSize; i++)
    {
        output->data[i] = a->data[i] * b->data[i];
    }
}


//Transpose the first matrix and multiply the two matrices 
void M_Transpose1Mul(const Matrix* a,const Matrix* b, Matrix* output)
{
#if DEBUG
    if(a->rows != b->rows || a->cols != output->rows || b->cols != output->cols)
    {
        M_Dim(a);
        M_Dim(b);
        M_Dim(output);
        errx(-1,"M_Transpose1Mul(): Matrix dimensions do not match\n");
        return;
    }
#endif

    for (size_t i = 0; i < a->cols; i++)
    {
        for (size_t j = 0; j < b->cols; j++)
        {
            output->data[i * output->cols + j] = 0;
            for (size_t k = 0; k < a->rows; ++k)
            {
                output->data[i * output->cols + j] += a->data[k * a->cols + i] * b->data[k * b->cols + j];
            }
        }
    }
}

//Transpose the second matrix and multiply the two matrices
void M_Transpose2Mul(const Matrix* a,const Matrix* b, Matrix* output)
{


#if DEBUG
    if(a->cols != b->cols || a->rows != output->rows || b->rows != output->cols)
    {
        printf("Output matrix dimensions must be: %li x %li\n",a->rows,b->rows);
        printf("Output matrix dimensions: %li x %li\n",output->rows,output->cols);
        printf("a dimensions: %li x %li\n",a->rows,a->cols);
        printf("b dimensions: %li x %li\n",b->rows,b->cols);
        errx(1,"M_Transpose2Mul(): Matrix dimensions do not match\n");
        return;
    }
#endif
    
    for (size_t i = 0; i < a->rows; i++)
    {
        for (size_t j = 0; j < b->rows; j++)
        {
            //output->data[i * output->cols + j] = 0;
            for (size_t k = 0; k < a->cols; k++)
            {
                output->data[i * output->cols + j] += a->data[i * a->cols + k] * b->data[j * b->cols + k];
            }
        }
    }
}

size_t M_GetSize3D(const Matrix* matrix)
{
    return matrix->cols * matrix->rows * matrix->dims;
}

size_t M_GetSize2D(const Matrix* matrix)
{
    return matrix->cols * matrix->rows;
}



void M_Copy(const Matrix* src, Matrix* dst)
{
    size_t matrixSize = M_GetSize3D(src);
    for (size_t i = 0; i < matrixSize; i++)
    {
        dst->data[i] = src->data[i];
    }
}

Matrix* M_CopyI(const Matrix* src)
{
    Matrix* dst = M_Create_3D(src->rows,src->cols,src->dims);
    M_Copy(src,dst);
    return dst;
}

void M_Set(Matrix* src, const float value)
{
    size_t matrixSize = M_GetSize3D(src);
    for (size_t i = 0; i < matrixSize; i++)
    {
        src->data[i] = value;
    }
}

void M_Zero(Matrix* src)
{
    M_Set(src,0);
}

Matrix* M_Transpose(const Matrix* m)
{
    Matrix* output = M_Create_2D(m->cols,m->rows);
    for (size_t i = 0; i < m->rows; i++)
    {
        for (size_t j = 0; j < m->cols; j++)
        {
            output->data[j * output->cols + i] = m->data[i * m->cols + j];
        }
    }
    return output;
}

int M_Equals(const Matrix* a, const Matrix* b)
{
    if(a->rows != b->rows || a->cols != b->cols || a->dims != b->dims)
    {
        return 0;
    }

    size_t matrixSize = M_GetSize3D(a);
    for (size_t i = 0; i < matrixSize; i++)
    {
        if(fabsf(a->data[i] - b->data[i]) > 0.001)
        {
            return 0;
        }
    }

    return 1;
}

void M_Save(const Matrix* m, FILE* file)
{
    fwrite(&m->rows,sizeof(size_t),1,file);
    fwrite(&m->cols,sizeof(size_t),1,file);
    fwrite(&m->effectiveRows,sizeof(size_t),1,file);
    fwrite(&m->effectiveCols,sizeof(size_t),1,file);
    fwrite(&m->dims,sizeof(size_t),1,file);
    fwrite(m->data,sizeof(float),M_GetSize3D(m),file);
}

size_t M_SaveSize(const Matrix* m)
{
    return sizeof(size_t) * 5 + sizeof(float) * M_GetSize3D(m);
}

size_t M_SaveSizeDim(size_t rows, size_t cols, size_t dims)
{
    return sizeof(size_t) * 5 + sizeof(float) * rows * cols * dims;
}

Matrix* M_Load(FILE* file)
{
    Matrix* m = (Matrix*)malloc(sizeof(Matrix));
    CheckRead(fread(&m->rows,sizeof(size_t),1,file));
    CheckRead(fread(&m->cols,sizeof(size_t),1,file));
    CheckRead(fread(&m->effectiveRows,sizeof(size_t),1,file));
    CheckRead(fread(&m->effectiveCols,sizeof(size_t),1,file));
    CheckRead(fread(&m->dims,sizeof(size_t),1,file));
    m->data = (float*)malloc(sizeof(float) * M_GetSize3D(m));
    CheckRead(fread(m->data,sizeof(float),M_GetSize3D(m),file));
    return m;
}


int M_GetMatrixType()
{
    return matrixType;
}

void SM_SetValue(Matrix* a, size_t index, float value)
{
    if(matrixType == 0)
    {
        a->data[index] = value;
    }
    else
    {
        BM_SetValue(a,index,value);
    }
}

void M_SetValue(Matrix* m,size_t rows, size_t cols, const float value)
{
    m->data[rows * m->cols + cols] = value;

}

float SM_GetValue(const Matrix* a, size_t index)
{
    if(matrixType == 0)
    {
        return a->data[index];
    }
    else
    {
        return BM_GetValue(a,index);
    }
}

void M_Convolution(const Matrix* a, const Matrix* b, Matrix* output)
{

#if DEBUG
    if(a->rows != output->rows + b->rows - 1 || a->cols != output->cols + b->cols - 1)
    {
        printf("Matrix output dimensions must be: %li x %li\n",a->rows - b->rows + 1,a->cols - b->cols + 1);
        printf("Actually : %li x %li\n",output->rows,output->cols);
        errx(-1,"M_Convolution(): Matrix dimensions do not match\n");
        return;
    }
#endif

    size_t outputRows = a->rows - b->rows + 1;
    size_t outputCols = a->cols - b->cols + 1;
    size_t outputDims = a->dims;
    size_t outputSize = outputRows * outputCols * outputDims;
    
    for (size_t i = 0; i < outputRows; i++)
    {
        for (size_t j = 0; j < outputCols; j++)
        {
            for (size_t k = 0; k < outputDims; k++)
            {
                float sum = 0;
                for (size_t l = 0; l < b->rows; l++)
                {
                    for (size_t m = 0; m < b->cols; m++)
                    {
                        sum += a->data[((i + l) * a->cols + (j + m)) * a->dims + k] * b->data[(l * b->cols + m) * b->dims + k];
                    }
                }
                output->data[(i * outputCols + j) * outputDims + k] = sum;
            }
        }
    }
}

void M_Convolution_ZeroPad(const Matrix* a, const Matrix* b, Matrix* output)
{
#if DEBUG
    if(a->rows != output->rows || a->cols != output->cols)
    {
        errx(-1,"M_Convolution_RepPad(): Matrix dimensions do not match\n");
        return;
    }
#endif
    size_t rowsToFill = b->rows / 2;
    size_t colsToFill = b->cols / 2;
    Matrix* filled = M_FillMatrixZero(a,rowsToFill,colsToFill);
    M_Convolution(filled,b,output);
    M_Free(filled);
}

void M_Convolution_Add(const Matrix* a, const Matrix* b, Matrix* output)
{
#if DEBUG
    if(a->rows != output->rows + b->rows - 1 || a->cols != output->cols + b->cols - 1)
    {
        printf("Matrix output dimensions must be: %li x %li\n",a->rows - b->rows + 1,a->cols - b->cols + 1);
        errx(-1,"M_Convolution_Add(): Matrix dimensions do not match\n");
        return;
    }
#endif

    for (int i = 0; i < output->rows; i++)
    {
        for (int j = 0; j < output->cols; j++)
        {
            float sum = 0;
            for (int k = 0; k < b->cols; k++)
            {
                for (int l = 0; l < b->rows; l++)
                {
                    sum += a->data[M_ConvertIndex(a,i + k, j + l)] * b->data[M_ConvertIndex(b,k,l)];
                }
            }
            output->data[(i * output->cols + j)] += sum;
        }
    }
}

void M_Convolution3D_Add(Matrix* a, Matrix* b, Matrix* output)
{
    float* aData = a->data;
    float* bData = b->data;
    float* outputData = output->data;
    size_t filterCount = b->dims / a->dims;

    for (size_t i = 0; i < a->dims; i++)
    {
        output->data = outputData;
        for (size_t j = 0; j < filterCount; j++)
        {
            M_Convolution_Add(a, b, output);
            b->data += M_GetSize2D(b);
            output->data += M_GetSize2D(output);
        }
        a->data += M_GetSize2D(a);
    }
    a->data = aData;
    b->data = bData;
    output->data = outputData;
}



void M_ReLU(const Matrix* input, Matrix* gradient, Matrix* output)
{
    size_t matrixSize = M_GetSize2D(input);
    for (size_t i = 0; i < matrixSize; i++)
    {
        if(input->data[i] > 0)
        {
            output->data[i] = input->data[i];
            gradient->data[i] = 1;
        }
        else
        {
            output->data[i] = 0;
            gradient->data[i] = 0;
        }
    }
    
}

void M_ReLU_Convolution(const Matrix* a, const Matrix* b,float bias, Matrix* deltaActivation, Matrix* output)
{
#if DEBUG

    if(a->rows != output->rows + b->rows - 1 || a->cols != output->cols + b->cols - 1)
    {
        printf("Matrix output dimensions must be: %li x %li\n",a->rows - b->rows + 1,a->cols - b->cols + 1);
        errx(-1,"M_ReLU_Convolution(): Matrix dimensions do not match\n");
        return;
    }
#endif


    for (int i = 0; i < output->rows; i++)
    {
        for (int j = 0; j < output->cols; j++)
        {
            float sum = 0;
            for (int k = 0; k < b->cols; k++)
            {
                for (int l = 0; l < b->rows; l++)
                {
                    sum += a->data[M_ConvertIndex(a,i + k, j + l)] * b->data[M_ConvertIndex(b,k,l)];
                }
            }

            sum = sum + bias;
            deltaActivation->data[(i * output->cols + j)] = sum > 0 ? 1 : 0;
            output->data[(i * output->cols + j)] = sum > 0 ? sum : 0;

        }
    }
}

void M_Convolution3D(Matrix* a, Matrix* b, Matrix* output)
{
    float* aData = a->data;
    float* bData = b->data;
    float* outputData = output->data;

    size_t filterCount = b->dims / a->dims;

    for (size_t i = 0; i < a->dims; i++)
    {
        for (size_t j = 0; j < filterCount; j++)
        {
            M_Convolution(a, b, output);
            b->data += M_GetSize2D(b);
            output->data += M_GetSize2D(output);
        }
        a->data += M_GetSize2D(a);
    }
    a->data = aData;
    b->data = bData;
    output->data = outputData;

}

void M_FullConvolution(Matrix* a, Matrix* b, Matrix* output)
{
    const int filterCols = b->cols;
    const int filterRows = b->rows;

    const int inputCols = a->cols;
    const int inputRows = a->rows;

    const int outputCols = a->cols + filterCols - 1;
    const int outputRows = a->rows + filterRows - 1;

    const int r = filterRows - 1;
    const int c = filterCols - 1;


    for (int i = 0; i < outputRows; i++)
    {
        for (int j = 0; j < outputCols; j++)
        {
            float sum = 0;
            for (int k = 0; k < filterRows; k++)
            {
                for (int l = 0; l < filterCols; l++)
                {
                    const int inputRow = i + k - r;
                    const int inputCol = j + l - c;
                    if (inputRow >= 0 && inputRow < inputRows && inputCol >= 0 && inputCol < inputCols)
                    {
                        sum += a->data[M_ConvertIndex(a,inputRow, inputCol)] * b->data[M_ConvertIndex(b,k,l)];
                    }
                }
            }
            output->data[M_ConvertIndex(output,i, j)] += sum;
        }
    }
}

void M_FullConvolution3D(Matrix* a, Matrix* b, Matrix* output) {
    float* aData = a->data;
    float* bData = b->data;
    float* outputData = output->data;
    M_Zero(output);

    size_t outputSliceSize = M_GetSize2D(output);
    size_t aSliceSize = M_GetSize2D(a);
    size_t numFilters = a->dims;
    size_t numFeatureMaps = output->dims;

    // Iterate over each filter in 'a'
    for (size_t j = 0; j < numFilters; j++) {
        // Temporary matrix to store the convolution result for the current filter
        Matrix* tempOutput = M_Create_2D(output->rows, output->cols);
        M_Zero(tempOutput);

        // Perform convolution for the current filter and delta activation
        M_FullConvolution(a, b, tempOutput);

        // Determine which feature map in 'output' this filter contributes to
        size_t featureMapIndex = j % numFeatureMaps;
        // Accumulate the result in the appropriate feature map of 'output'
        for (size_t k = 0; k < outputSliceSize; k++) {
            output->data[featureMapIndex * outputSliceSize + k] += tempOutput->data[k];
        }

        // Move to the next filter and the corresponding delta activation slice
        a->data += aSliceSize;
        b->data += M_GetSize2D(b);

        // Clean up temporary matrix
        M_Free(tempOutput);
    }

    // Reset data pointers to original position
    a->data = aData;
    b->data = bData;
    output->data = outputData;
}

void M_Rotate180(Matrix* m, Matrix* output)
{
    for (size_t i = 0; i < m->rows; i++)
    {
        for (size_t j = 0; j < m->cols; j++)
        {
            size_t new_i = m->rows - 1 - i;
            size_t new_j = m->cols - 1 - j;
            
            output->data[new_i * output->cols + new_j] = m->data[i * m->cols + j];
        }
    }
}

void M_Rotate180_3D(Matrix* m, Matrix* output)
{
    float* mData = m->data;
    float* outputData = output->data;

    size_t filterCount = m->dims;

    for (size_t i = 0; i < filterCount; i++)
    {
        M_Rotate180(m, output);
        m->data += M_GetSize2D(m);
        output->data += M_GetSize2D(output);
    }
    m->data = mData;
    output->data = outputData;
}


float M_GetSum(const Matrix* m)
{
    float sum = 0;
    for (size_t i = 0; i < M_GetSize3D(m); i++)
    {
        sum += m->data[i];
    }
    return sum;
}


void M_MaxPool(const Matrix* m, Matrix* output, size_t* maxIndexes, size_t size, size_t id)
{
    size_t i = 0;
    for (size_t r = 0; r < m->rows; r+=size)
    {
        for (size_t c = 0; c < m->cols; c+=size)
        {
            size_t maxIndex = 0;
            float max = FLT_MAX * -1;
            for (size_t i = 0; i < size; i++)
            {
                for (size_t j = 0; j < size; j++)
                {
                    size_t index = M_ConvertIndex(m,r + i,c + j);
                    if(m->data[index] > max)
                    {
                        max = m->data[index];
                        maxIndex = index;
                    }
                }
            }
            size_t outputIndex = M_ConvertIndex(output,r / size,c / size);
            output->data[outputIndex] = max;
            maxIndexes[i + id * M_GetSize2D(output)] = maxIndex + id * M_GetSize2D(m);
            i++;
        }
    }
}


void M_MaxPool3D(Matrix* m, Matrix* output, size_t* maxIndexes, size_t size)
{
    float* mData = m->data;
    float* outputData = output->data;

    size_t filterCount = m->dims;


    for (size_t i = 0; i < filterCount; i++)
    {
        M_MaxPool(m, output, maxIndexes, size, i);
        m->data += M_GetSize2D(m);
        output->data += M_GetSize2D(output);
    }
    
    m->data = mData;
    output->data = outputData;
}


void M_Convolution3D_2D_Add(Matrix* a, Matrix* b, Matrix* output)
{
    float* aData = a->data;
    for (size_t i = 0; i < a->dims; i++)
    {
        M_Convolution_Add(a, b, output);
        a->data += M_GetSize2D(a);
    }
    a->data = aData;
}

void M_AddBiasReLU(Matrix* input, float bias, Matrix* deltaActivation, Matrix* output)
{
    for (size_t i = 0; i < M_GetSize2D(input); i++)
    {
        float res = input->data[i] + bias;
        deltaActivation->data[i] = res > 0 ? 1 : 0;
        output->data[i] = res > 0 ? res : 0;
    }
}

float two_by_two_det(Matrix* m)
{
    return m->data[0] * m->data[3] - m->data[1] * m->data[2];
}


Matrix* M_Complete(const Matrix* m, size_t rows, size_t cols)
{
    size_t rowsToAdd = rows - m->rows;
    size_t colsToAdd = cols - m->cols;
    Matrix* res = M_Create_2D(rows,cols);
    for (size_t i = 0; i < m->rows; i++)
    {
        for (size_t j = 0; j < m->cols; j++)
        {
            res->data[i*cols+j] = m->data[i * m->cols + j];
        }
    }
    return res;
}


void M_DepthwiseConvolution(Matrix* input, Matrix* filter, Matrix* output)
{
    float* filterData = filter->data;

    for (size_t i = 0; i < input->rows; i++)
    {
        for (size_t j = 0; j < input->cols; j++)
        {
            for (size_t l = 0; l < filter->dims; l++)
            {
                float sum = 0;
                for (size_t k = 0; k < filter->rows; k++)
                {
                    for (size_t m = 0; m < filter->cols; m++)
                    {
                        sum += input->data[M_ConvertIndex(input,i + k, j + m) + l * M_GetSize2D(input)] * *filterData;
                        filterData++;
                    }
                }
                output->data[M_ConvertIndex(output,i,j) + l * M_GetSize2D(output)] = sum;
            }
        }
    }
}

void M_PointWiseConvolution(Matrix* input, Matrix* filter, Matrix* output)
{
    float* filterData = filter->data;

    for (size_t i = 0; i < input->rows; i++)
    {
        for (size_t j = 0; j < input->cols; j++)
        {
            for (size_t l = 0; l < filter->dims; l++)
            {
                float sum = 0;
                for (size_t k = 0; k < filter->rows; k++)
                {
                    for (size_t m = 0; m < filter->cols; m++)
                    {
                        sum += input->data[M_ConvertIndex(input,i + k, j + m) + l * M_GetSize2D(input)] * *filterData;
                        filterData++;
                    }
                }
                output->data[M_ConvertIndex(output,i,j) + l * M_GetSize2D(output)] = sum;
            }
        }
    }
}
