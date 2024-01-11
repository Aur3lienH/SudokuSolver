#include "matrix/Matrix.h"
#include "deepLearning/Allocator.h"
#include <math.h>
#include <err.h>
#include <string.h>
#ifdef __arm__
    #define SSE 0
#elif defined(__aarch64__)
    #define SSE 0
#else
    #define SSE 1
    #define AVX 1
    #include <immintrin.h>
    #include <emmintrin.h>
    #include <xmmintrin.h>
#endif




#define DEBUG 1

const size_t BLOCK_SIZE = 32;
const size_t BLOCK_SIZE_SQUARED = BLOCK_SIZE * BLOCK_SIZE;
const size_t SSE_OPERATION_COUNT = BLOCK_SIZE / 4;
const size_t AVX_OPERATION_COUNT = BLOCK_SIZE / 8;
const size_t PREFETCH_DISTANCE = 4;



Matrix* BM_Create_3D(size_t rows, size_t cols, size_t dims)
{
    size_t finalRows = rows;
    size_t finalCols = cols;
    while(finalRows % BLOCK_SIZE != 0)
    {
        finalRows++;
    }
    while(finalCols % BLOCK_SIZE != 0)
    {
        finalCols++;
    }
    return M_Init(finalRows,finalCols,dims,rows,cols);
}

Matrix* BM_Create_2D(size_t rows, size_t cols)
{
    return BM_Create_3D(rows,cols,1);
}

void BM_Mul(const Matrix* a, const Matrix* b, Matrix* c)
{
#if DEBUG
    if(a->cols != b->rows || a->rows != c->rows || b->cols != c->cols)
    {
        errx(-1,"M_Mul(): Matrix dimensions do not match\n");
        return;
    }
#endif
    float* left = a->data;
    float* right = b->data;
    float* out = c->data;

    const size_t blockSizeSquared = BLOCK_SIZE * BLOCK_SIZE;
    const size_t rowsRightBlockCount = b->rows / BLOCK_SIZE;
    const size_t colsRightBlockCount = b->cols / BLOCK_SIZE;
    const size_t rowsLeftBlockCount = a->rows / BLOCK_SIZE;
    M_Zero(c);

    for (size_t f = 0; f < rowsLeftBlockCount; f++)
    {
        right = b->data;
        for(size_t i = 0; i < rowsRightBlockCount; i++)
        {
            
            for (size_t j = 0; j < colsRightBlockCount; j++)
            {
                for (size_t ii = 0; ii < BLOCK_SIZE; ii++)
                {
                    for (size_t jj = 0; jj < BLOCK_SIZE; jj++)
                    {
                        float sum = 0.0f;
                        float* res = (out + ii * BLOCK_SIZE + jj);

                        for (size_t kk = 0; kk < BLOCK_SIZE; kk++)
                        {
                            sum += left[ii * BLOCK_SIZE + kk] * right[kk * BLOCK_SIZE + jj];
                        }
                        *res += sum;
                    }
                }

                right += blockSizeSquared;
                out += blockSizeSquared;
            }
            out -= blockSizeSquared * colsRightBlockCount; 
            left += blockSizeSquared;
        }   
        out += blockSizeSquared * colsRightBlockCount;

    }
}


void BM_M_Mul(const Matrix* a, const Matrix* b, Matrix* c)
{
#if DEBUG
    if(a->effectiveCols != b->rows || a->effectiveRows != c->rows || b->cols != c->cols || b->cols != 1)
    {
        errx(-1,"BM_M_Mul: Matrix dimensions do not match\n");
        return;
    }
#endif
    float* left = a->data;
    float* right = b->data;
    float* out = c->data;

    const size_t blockSizeSquared = BLOCK_SIZE * BLOCK_SIZE;
    const size_t rowsRightBlockCount = a->cols / BLOCK_SIZE;
    const size_t rowsLeftBlockCount = a->rows / BLOCK_SIZE;

    size_t missingOutCols = c->rows % BLOCK_SIZE;
    size_t aMissingRows = b->rows % BLOCK_SIZE;
    if(aMissingRows == 0)
    {
        aMissingRows = BLOCK_SIZE;
    }
    if(missingOutCols == 0)
    {
        missingOutCols = BLOCK_SIZE;
    }
    M_Zero(c);
    //Set output to 0
    for (size_t f = 0; f < rowsLeftBlockCount; f++)
    {
        size_t outLoopCount = (f == rowsLeftBlockCount - 1) ? missingOutCols : BLOCK_SIZE;
        right = b->data;
        for(size_t i = 0; i < rowsRightBlockCount; i++)
        {
            size_t leftLoopCount = (i == rowsRightBlockCount - 1) ? aMissingRows : BLOCK_SIZE;

            for (size_t jj = 0; jj < outLoopCount; jj++)
            {
                float* res = (out + jj);
                float sum = 0;
                for (size_t kk = 0; kk < leftLoopCount; kk++)
                {
                    sum += left[kk + jj * BLOCK_SIZE] * right[kk];
                }
                *res += sum;
            }
            right += BLOCK_SIZE;
            left += blockSizeSquared;
        }   
        out += BLOCK_SIZE;

    }
}


void BM_M_SSE_Mul(const Matrix* a, const Matrix* b, Matrix* c)
{
#if DEBUG
    if(a->effectiveCols != b->rows || a->effectiveRows != c->rows || b->cols != c->cols || b->cols != 1)
    {
        M_Dim(a);
        M_Dim(b);
        M_Dim(c);
        errx(-1,"BM_M_SSE_Mul: Matrix dimensions do not match\n");
        return;
    }
    if(BLOCK_SIZE % 4 != 0)
    {
        errx(-1,"BM_M_SSE_Transpose1Mul: BLOCK_SIZE must be a multiple of 4\n");
        return;
    }
#endif
    float* left = a->data;
    float* right = b->data;
    float* out = c->data;

    size_t blockSizeSquared = BLOCK_SIZE * BLOCK_SIZE;
    size_t rowsRightBlockCount = a->cols / BLOCK_SIZE;
    size_t rowsLeftBlockCount = a->rows / BLOCK_SIZE;

    size_t missingOutCols = c->rows % BLOCK_SIZE;
    size_t aMissingRows = b->rows % BLOCK_SIZE;
    if(aMissingRows == 0)
    {
        aMissingRows = BLOCK_SIZE;
    }
    if(missingOutCols == 0)
    {
        missingOutCols = BLOCK_SIZE;
    }
    M_Zero(c);
    //Set output to 0
    //The two loops are used to go through the blocks of the matrices
    for (size_t f = 0; f < rowsLeftBlockCount; f++)
    {
        size_t outLoopCount = (f == rowsLeftBlockCount - 1) ? missingOutCols : BLOCK_SIZE;
        right = b->data;
        for(size_t i = 0; i < rowsRightBlockCount; i++)
        {
            size_t leftLoopCount = (i == rowsRightBlockCount - 1) ? aMissingRows : BLOCK_SIZE;

            
            //Go to the next row in this matrix and go to the next column in the other matrix
            for (size_t jj = 0; jj < outLoopCount; jj++)
            {
                float* res = (out + jj);
                float sum = 0;
                size_t kk = 0;
                //Go to the next column in this matrix and go to the next rows in the other matrix
#if SSE
                
                __m128 sumVector = _mm_setzero_ps();
                for (; kk + 4 <= leftLoopCount; kk += 4)
                {
                    __m128 leftVector = _mm_load_ps(left + kk + jj * BLOCK_SIZE);
                    __m128 rightVector = _mm_load_ps(right + kk);
                    __m128 mulVector = _mm_mul_ps(leftVector,rightVector);
                    sumVector = _mm_add_ps(sumVector,mulVector);

                }
                sumVector = _mm_hadd_ps(sumVector,sumVector);
                sumVector = _mm_hadd_ps(sumVector,sumVector);
                _mm_store_ss(&sum,sumVector);
#endif
                for (; kk < leftLoopCount; kk++)
                {
                    sum += left[kk + jj * BLOCK_SIZE] * right[kk];
                }
                
                *res += sum;
            }
            right += BLOCK_SIZE;
            left += blockSizeSquared;
        }   
        out += BLOCK_SIZE;

    }
}




void BM_M_SSE_Mul_Loop(float* left, float* right, float* out,const size_t jj, const size_t leftLoopCount)
{
    float* res = (out + jj);
    float sum = 0;
    size_t kk = 0;
                //Go to the next column in this matrix and go to the next rows in the other matrix
#if SSE
    __m128 sumVector = _mm_setzero_ps();
    for (; kk + 4 <= leftLoopCount; kk += 4)
    {
        __m128 leftVector = _mm_load_ps(left + kk + jj * BLOCK_SIZE);
        __m128 rightVector = _mm_load_ps(right + kk);
        sumVector = _mm_add_ps(sumVector,_mm_mul_ps(leftVector,rightVector));

    }
    sumVector = _mm_hadd_ps(sumVector,sumVector);
    sumVector = _mm_hadd_ps(sumVector,sumVector);
    _mm_store_ss(&sum,sumVector);
#endif
    for (; kk < leftLoopCount; kk++)
    {
        sum += left[kk + jj * BLOCK_SIZE] * right[kk];
    }
    
    *res += sum;
}

void BM_M_SSE_Mul_Loop2(float* left, float* right, float* out,size_t jj)
{
    float* res = (out + jj);
    float sum = 0;
                //Go to the next column in this matrix and go to the next rows in the other matrix
#if SSE
    __m128 sumVector = _mm_setzero_ps();
    __m128 leftVector = _mm_load_ps(left + jj * BLOCK_SIZE);
    __m128 rightVector = _mm_load_ps(right);
    for (size_t kk = 0; kk < SSE_OPERATION_COUNT; kk++)
    {
        sumVector = _mm_add_ps(sumVector,_mm_mul_ps(leftVector,rightVector));

        leftVector = _mm_load_ps(left + ((kk + 1) * 4) + jj * BLOCK_SIZE);
        rightVector = _mm_load_ps(right + ((kk+1) * 4));
    }
    sumVector = _mm_hadd_ps(sumVector,sumVector);
    sumVector = _mm_hadd_ps(sumVector,sumVector);
    _mm_store_ss(&sum,sumVector);
#endif
    *res += sum;
}


void BM_M_SSE_NCOND_Mul(const Matrix* a, const Matrix* b, Matrix* c)
{
#if DEBUG
    if(a->effectiveCols != b->rows || a->effectiveRows != c->rows || b->cols != c->cols || b->cols != 1)
    {
        errx(-1,"BM_M_SSE_NCOND_Mul: Matrix dimensions do not match\n");
        return;
    }
    if(BLOCK_SIZE % 4 != 0)
    {
        errx(-1,"BM_M_SSE_Transpose1Mul: BLOCK_SIZE must be a multiple of 4\n");
        return;
    }
#endif
    float* left = a->data;
    float* right = b->data;
    float* out = c->data;

    size_t blockSizeSquared = BLOCK_SIZE * BLOCK_SIZE;
    size_t rowsRightBlockCount = (a->cols / BLOCK_SIZE)-1;
    size_t rowsLeftBlockCount = (a->rows / BLOCK_SIZE)-1;

    size_t missingOutCols = c->rows % BLOCK_SIZE;
    size_t aMissingRows = b->rows % BLOCK_SIZE;
    if(aMissingRows == 0)
    {
        aMissingRows = BLOCK_SIZE;
    }
    if(missingOutCols == 0)
    {
        missingOutCols = BLOCK_SIZE;
    }
    //M_Zero(c);
    //Set output to 0
    //The two loops are used to go through the blocks of the matrices
    for (size_t f = 0; f < rowsLeftBlockCount; f++)
    {
        right = b->data;
        for(size_t i = 0; i < rowsRightBlockCount; i++)
        {
            //Go to the next row in this matrix and go to the next column in the other matrix
            for (size_t jj = 0; jj < BLOCK_SIZE; jj++)
            {
                BM_M_SSE_Mul_Loop2(left,right,out,jj);
            }
            
            right += BLOCK_SIZE;
            left += blockSizeSquared;
        }

        for (size_t jj = 0; jj < BLOCK_SIZE; jj++)
        {
            BM_M_SSE_Mul_Loop(left,right,out,jj,aMissingRows);
        }
        right += BLOCK_SIZE;
        left += blockSizeSquared;
        
        
        out += BLOCK_SIZE;

    }

    right = b->data;
    for(size_t i = 0; i < rowsRightBlockCount; i++)
    {
        //Go to the next row in this matrix and go to the next column in the other matrix
        for (size_t jj = 0; jj < missingOutCols; jj++)
        {
            BM_M_SSE_Mul_Loop2(left,right,out,jj);
        }
        
        right += BLOCK_SIZE;
        left += blockSizeSquared;
    }

    for (size_t jj = 0; jj < missingOutCols; jj++)
    {
        BM_M_SSE_Mul_Loop(left,right,out,jj,aMissingRows);
    }
}

void BM_M_AVX_Mul(const Matrix* a, const Matrix* b, Matrix* c)
{
#if DEBUG
    if(a->effectiveCols != b->rows || a->effectiveRows != c->rows || b->cols != c->cols || b->cols != 1)
    {
        errx(-1,"BM_M_AVX_Mul: Matrix dimensions do not match\n");
        return;
    }
    if(BLOCK_SIZE % 8 != 0)
    {
        errx(-1,"BM_M_AVX_Mul: BLOCK_SIZE must be a multiple of 8\n");
        return;
    }
#endif
    float* __restrict left = a->data;
    float* __restrict right = b->data;
    float* __restrict out = c->data;

    size_t blockSizeSquared = BLOCK_SIZE * BLOCK_SIZE;
    size_t rowsRightBlockCount = a->cols / BLOCK_SIZE;
    size_t rowsLeftBlockCount = a->rows / BLOCK_SIZE;

    size_t missingOutCols = c->rows % BLOCK_SIZE;
    size_t aMissingRows = b->rows % BLOCK_SIZE;
    if(aMissingRows == 0)
    {
        aMissingRows = BLOCK_SIZE;
    }
    if(missingOutCols == 0)
    {
        missingOutCols = BLOCK_SIZE;
    }
    M_Zero(c);
    //Set output to 0
    //The two loops are used to go through the blocks of the matrices
    for (size_t f = 0; f < rowsLeftBlockCount; f++)
    {
        size_t outLoopCount = (f == rowsLeftBlockCount - 1) ? missingOutCols : BLOCK_SIZE;
        right = b->data;
        for(size_t i = 0; i < rowsRightBlockCount; i++)
        {
            size_t leftLoopCount = (i == rowsRightBlockCount - 1) ? aMissingRows : BLOCK_SIZE;
#if AVX
            _mm_prefetch(right, _MM_HINT_T0);
#endif
            //Go to the next row in this matrix and go to the next column in the other matrix
            for (size_t jj = 0; jj < outLoopCount; jj++)
            {
                float* res = (out + jj);
                size_t kk = 0;
                //Go to the next column in this matrix and go to the next rows in the other matrix
#if AVX         
                __m256 sumVector = _mm256_setzero_ps();
                for (; kk + 8 <= leftLoopCount; kk += 8)
                {
                    __m256 leftVector = _mm256_load_ps(left + kk + jj * BLOCK_SIZE);
                    __m256 rightVector = _mm256_load_ps(right + kk);
                    sumVector = _mm256_fmadd_ps(leftVector,rightVector,sumVector);

                }
                sumVector = _mm256_hadd_ps(sumVector,sumVector);
                sumVector = _mm256_hadd_ps(sumVector,sumVector);
                float sum = _mm256_cvtss_f32(_mm256_permute2f128_ps(sumVector, sumVector, 0x81));
                sum += _mm256_cvtss_f32(sumVector); // Add the high 128

#else
                float sum = 0;
#endif
                for (; kk < leftLoopCount; kk++)
                {
                    sum += left[kk + jj * BLOCK_SIZE] * right[kk];
                }
                
                *res += sum;
            }
            right += BLOCK_SIZE;
            left += blockSizeSquared;
        }   
        out += BLOCK_SIZE;

    }
}

void BM_M_AVX_Mul_Loop(float* left, float* right, float* out,size_t jj, size_t leftLoopCount)
{
    float* res = (out + jj);
    float sum = 0;
    size_t kk = 0;
                //Go to the next column in this matrix and go to the next rows in the other matrix
#if AVX
    __m256 sumVector = _mm256_setzero_ps();
    for (; kk + 8 <= leftLoopCount; kk += 8)
    {
        __m256 leftVector = _mm256_load_ps(left + kk + jj * BLOCK_SIZE);
        __m256 rightVector = _mm256_load_ps(right + kk);
        sumVector = _mm256_add_ps(sumVector,_mm256_mul_ps(leftVector,rightVector));

    }
    sumVector = _mm256_hadd_ps(sumVector,sumVector);
    sumVector = _mm256_hadd_ps(sumVector,sumVector);
    sum += sumVector[0] + sumVector[4];
#endif
    for (; kk < leftLoopCount; kk++)
    {
        sum += left[kk + jj * BLOCK_SIZE] * right[kk];
    }
    
    *res += sum;
}

void BM_M_AVX_Mul_Loop2(float* left, float* right, float* out,size_t jj)
{
    float* res = (out + jj);
    float sum = 0;
                //Go to the next column in this matrix and go to the next rows in the other matrix
#if AVX
    __m256 sumVector = _mm256_setzero_ps();
    __m256 leftVector = _mm256_load_ps(left  + jj * BLOCK_SIZE);
    __m256 rightVector = _mm256_load_ps(right);
    for (size_t kk = 0; kk < AVX_OPERATION_COUNT; kk++)
    {
        sumVector = _mm256_add_ps(sumVector,_mm256_mul_ps(leftVector,rightVector));
        leftVector = _mm256_load_ps(left + ((kk+1)*8) + jj * BLOCK_SIZE);
        rightVector = _mm256_load_ps(right + ((kk+1)*8));
    }
    sumVector = _mm256_hadd_ps(sumVector,sumVector);
    sumVector = _mm256_hadd_ps(sumVector,sumVector);
    sum += sumVector[0] + sumVector[4];
#endif
    
    *res += sum;
}


void BM_M_AVX_NCOND_Mul(const Matrix* a, const Matrix* b, Matrix* c)
{
    #if DEBUG
    if(a->effectiveCols != b->rows || a->effectiveRows != c->rows || b->cols != c->cols || b->cols != 1)
    {
        errx(-1,"BM_M_AVX_NCOND_Mul: Matrix dimensions do not match\n");
        return;
    }
    if(BLOCK_SIZE % 8 != 0)
    {
        errx(-1,"BM_M_AVX_NCOND_Mul: BLOCK_SIZE must be a multiple of 8\n");
        return;
    }
#endif
    float* left = a->data;
    float* right = b->data;
    float* out = c->data;

    size_t blockSizeSquared = BLOCK_SIZE * BLOCK_SIZE;
    size_t rowsRightBlockCount = (a->cols / BLOCK_SIZE)-1;
    size_t rowsLeftBlockCount = (a->rows / BLOCK_SIZE)-1;

    size_t missingOutCols = c->rows % BLOCK_SIZE;
    size_t aMissingRows = b->rows % BLOCK_SIZE;
    if(aMissingRows == 0)
    {
        aMissingRows = BLOCK_SIZE;
    }
    if(missingOutCols == 0)
    {
        missingOutCols = BLOCK_SIZE;
    }
    //M_Zero(c);
    //Set output to 0
    //The two loops are used to go through the blocks of the matrices
    for (size_t f = 0; f < rowsLeftBlockCount; f++)
    {
        right = b->data;
        for(size_t i = 0; i < rowsRightBlockCount; i++)
        {
            //Go to the next row in this matrix and go to the next column in the other matrix
            for (size_t jj = 0; jj < BLOCK_SIZE; jj++)
            {
                BM_M_AVX_Mul_Loop2(left,right,out,jj);
            }
            
            right += BLOCK_SIZE;
            left += blockSizeSquared;
        }

        for (size_t jj = 0; jj < BLOCK_SIZE; jj++)
        {
            BM_M_AVX_Mul_Loop(left,right,out,jj,aMissingRows);
        }
        right += BLOCK_SIZE;
        left += blockSizeSquared;
        
        
        out += BLOCK_SIZE;

    }

    right = b->data;
    for(size_t i = 0; i < rowsRightBlockCount; i++)
    {
        //Go to the next row in this matrix and go to the next column in the other matrix
        for (size_t jj = 0; jj < missingOutCols; jj++)
        {
            BM_M_AVX_Mul_Loop2(left,right,out,jj);
        }
        
        right += BLOCK_SIZE;
        left += blockSizeSquared;
    }

    for (size_t jj = 0; jj < missingOutCols; jj++)
    {
        BM_M_AVX_Mul_Loop(left,right,out,jj,aMissingRows);
    }
}

void BM_M_Transpose1Mul(const Matrix* a, const Matrix* b, Matrix* c)
{
    float* left = a->data;
    float* right = b->data;
    float* out = c->data;


    size_t rowsRightBlockCount = a->rows / BLOCK_SIZE;
    size_t colsLeftBlockCount = a->cols / BLOCK_SIZE;

    
    size_t missingOutRows = c->rows % BLOCK_SIZE;
    size_t missingRightRows = b->rows % BLOCK_SIZE;

    if(missingOutRows == 0)
    {
        missingOutRows = BLOCK_SIZE;
    }
    if(missingRightRows == 0)
    {
        missingRightRows = BLOCK_SIZE;
    }

    M_Zero(c);


    // The two loops are used to go through the blocks of the matrices
    for (size_t f = 0; f < colsLeftBlockCount; f++)
    {
        right = b->data;
        size_t outLoopCount = (f == colsLeftBlockCount - 1) ? missingOutRows : BLOCK_SIZE;
        for (size_t i = 0; i < rowsRightBlockCount; i++) 
        {
            size_t rightLoopCount = (i == rowsRightBlockCount - 1) ? missingRightRows : BLOCK_SIZE;
            for (size_t ii = 0; ii < outLoopCount; ii++)
            {
                float* res = (out + ii);
                float sum = 0;
                for (size_t kk = 0; kk < rightLoopCount; kk++)
                {
                    // Go to the next column in matrix A (because it's transposed) 
                    // and go to the next row in matrix B

                    sum += left[kk * BLOCK_SIZE + ii] * right[kk]; 
                }
                *res += sum;
                
            }

            right += BLOCK_SIZE;
            left += BLOCK_SIZE_SQUARED * colsLeftBlockCount;  
        }
        left -= BLOCK_SIZE_SQUARED * rowsRightBlockCount * colsLeftBlockCount;
        left += BLOCK_SIZE_SQUARED;
        out += BLOCK_SIZE;
    }
}



void BM_Transpose1Mul(const Matrix* a,const Matrix* b, Matrix* c)
{

    float* left = a->data;
    float* right = b->data;
    float* out = c->data;

    size_t rowsRightBlockCount = b->rows / BLOCK_SIZE;
    size_t colsRightBlockCount = b->cols / BLOCK_SIZE;
    size_t colsLeftBlockCount = a->cols / BLOCK_SIZE;
    M_Zero(c);

    // The two loops are used to go through the blocks of the matrices
    for (size_t f = 0; f < colsLeftBlockCount; f++)
    {
        right = b->data;
        for (size_t i = 0; i < rowsRightBlockCount; i++) 
        {
            for (size_t j = 0; j < colsRightBlockCount; j++)
            {
                for (size_t ii = 0; ii < BLOCK_SIZE; ii++)
                {
                    for (size_t jj = 0; jj < BLOCK_SIZE; jj++)
                    {
                        float* res = (out + jj + ii * BLOCK_SIZE);
                        size_t kk = 0;
                        for (; kk < BLOCK_SIZE; kk++)
                        {
                            // Go to the next column in matrix A (because it's transposed) 
                            // and go to the next row in matrix B
                            *res += left[kk * BLOCK_SIZE + ii] * right[kk * BLOCK_SIZE + jj]; 
                        }
                    }
                }

                right += BLOCK_SIZE_SQUARED;
                out += BLOCK_SIZE_SQUARED;
            }

            out -= BLOCK_SIZE_SQUARED * colsRightBlockCount;
            left += BLOCK_SIZE_SQUARED * colsLeftBlockCount;  
        }
        left -= BLOCK_SIZE_SQUARED * rowsRightBlockCount * colsLeftBlockCount;
        left += BLOCK_SIZE_SQUARED;
        out += BLOCK_SIZE_SQUARED * colsRightBlockCount;
    }
}

void BM_M_SSE_Transpose1Mul(const Matrix* a, const Matrix* b, Matrix* c)
{
#if DEBUG
    if(BLOCK_SIZE % 4 != 0)
    {
        errx(-1,"BM_M_SSE_Transpose1Mul: BLOCK_SIZE must be a multiple of 4\n");
        return;
    }

    if(a->rows != b->rows || a->cols != c->rows || b->cols != c->cols)
    {
        errx(-1,"BM_M_SSE_Transpose1Mul(): Matrix dimensions do not match\n");
        return;
    }
#endif

    float* left = a->data;
    float* right = b->data;
    float* out = c->data;


    size_t rowsRightBlockCount = a->rows / BLOCK_SIZE;
    size_t colsLeftBlockCount = a->cols / BLOCK_SIZE;

    
    size_t missingOutRows = c->rows % BLOCK_SIZE;
    size_t missingRightRows = b->rows % BLOCK_SIZE;

    if(missingOutRows == 0)
    {
        missingOutRows = BLOCK_SIZE;
    }
    if(missingRightRows == 0)
    {
        missingRightRows = BLOCK_SIZE;
    }

    M_Zero(c);


    // The two loops are used to go through the blocks of the matrices
    for (size_t f = 0; f < colsLeftBlockCount; f++)
    {
        right = b->data;
        size_t outLoopCount = (f == colsLeftBlockCount - 1) ? missingOutRows : BLOCK_SIZE;
        for (size_t i = 0; i < rowsRightBlockCount; i++) 
        {
            size_t rightLoopCount = (i == rowsRightBlockCount - 1) ? missingRightRows : BLOCK_SIZE;
            for (size_t ii = 0; ii < outLoopCount; ii++)
            {
                float* res = (out + ii);
                float sum = 0;
                size_t kk = 0;
#if SSE
                __m128 sumVector = _mm_setzero_ps();
                for (; kk + 4 <= rightLoopCount; kk += 4)
                {
                    // Go to the next column in matrix A (because it's transposed) 
                    // and go to the next row in matrix B
                    __m128 leftVector = _mm_set_ps(left[(kk+3) * BLOCK_SIZE + ii],left[(kk + 2) * BLOCK_SIZE + ii],left[(kk + 1) * BLOCK_SIZE + ii],left[kk * BLOCK_SIZE + ii]);
                    __m128 rightVector = _mm_loadu_ps(right + kk);
                    __m128 mulVector = _mm_mul_ps(leftVector,rightVector);
                    sumVector = _mm_add_ps(sumVector,mulVector);
                }
                sumVector = _mm_hadd_ps(sumVector,sumVector);
                sumVector = _mm_hadd_ps(sumVector,sumVector);
                _mm_store_ss(&sum,sumVector);

#endif


                for (; kk < rightLoopCount; kk++)
                {
                    // Go to the next column in matrix A (because it's transposed) 
                    // and go to the next row in matrix B

                    sum += left[kk * BLOCK_SIZE + ii] * right[kk]; 
                }
                *res += sum;
                
            }

            right += BLOCK_SIZE;
            left += BLOCK_SIZE_SQUARED * colsLeftBlockCount;  
        }
        left -= BLOCK_SIZE_SQUARED * rowsRightBlockCount * colsLeftBlockCount;
        left += BLOCK_SIZE_SQUARED;
        out += BLOCK_SIZE;
    }
}

void BM_M_AVX_Transpose1Mul(const Matrix* a, const Matrix* b, Matrix* c)
{
    #if DEBUG
    if(BLOCK_SIZE % 4 != 0)
    {
        errx(-1,"BM_M_AVX_Transpose1Mul: BLOCK_SIZE must be a multiple of 4\n");
        return;
    }

    if(a->effectiveRows != b->effectiveRows || a->effectiveCols != c->effectiveRows || b->effectiveCols != c->effectiveCols)
    {
        M_Dim(a);
        M_Dim(b);
        M_Dim(c);
        errx(-1,"BM_M_AVX_Transpose1Mul(): Matrix dimensions do not match\n");
        return;
    }
#endif

    float* left = a->data;
    float* right = b->data;
    float* out = c->data;


    size_t rowsRightBlockCount = a->rows / BLOCK_SIZE;
    size_t colsLeftBlockCount = a->cols / BLOCK_SIZE;

    
    size_t missingOutRows = c->rows % BLOCK_SIZE;
    size_t missingRightRows = b->rows % BLOCK_SIZE;

    if(missingOutRows == 0)
    {
        missingOutRows = BLOCK_SIZE;
    }
    if(missingRightRows == 0)
    {
        missingRightRows = BLOCK_SIZE;
    }

    M_Zero(c);


    // The two loops are used to go through the blocks of the matrices
    for (size_t f = 0; f < colsLeftBlockCount; f++)
    {
        right = b->data;
        size_t outLoopCount = (f == colsLeftBlockCount - 1) ? missingOutRows : BLOCK_SIZE;
        for (size_t i = 0; i < rowsRightBlockCount; i++) 
        {
            size_t rightLoopCount = (i == rowsRightBlockCount - 1) ? missingRightRows : BLOCK_SIZE;
            for (size_t ii = 0; ii < outLoopCount; ii++)
            {
                float* res = (out + ii);
                float sum = 0;
                size_t kk = 0;
#if SSE
                __m256 sumVector = _mm256_setzero_ps();
                for (; kk + 8 <= rightLoopCount; kk += 8)
                {
                    // Go to the next column in matrix A (because it's transposed) 
                    // and go to the next row in matrix B
                    __m256 leftVector = _mm256_set_ps(left[(kk+7) * BLOCK_SIZE + ii],left[(kk + 6) * BLOCK_SIZE + ii],left[(kk + 5) * BLOCK_SIZE + ii],left[(kk+4) * BLOCK_SIZE + ii],left[(kk+3) * BLOCK_SIZE + ii],left[(kk + 2) * BLOCK_SIZE + ii],left[(kk + 1) * BLOCK_SIZE + ii],left[kk * BLOCK_SIZE + ii]);
                    __m256 rightVector = _mm256_loadu_ps(right + kk);
                    __m256 mulVector = _mm256_mul_ps(leftVector,rightVector);
                    sumVector = _mm256_add_ps(sumVector,mulVector);
                }
                sumVector = _mm256_hadd_ps(sumVector,sumVector);
                sumVector = _mm256_hadd_ps(sumVector,sumVector);
                sum += sumVector[0] + sumVector[4];

#endif


                for (; kk < rightLoopCount; kk++)
                {
                    // Go to the next column in matrix A (because it's transposed) 
                    // and go to the next row in matrix B

                    sum += left[kk * BLOCK_SIZE + ii] * right[kk]; 
                }
                *res += sum;
                
            }

            right += BLOCK_SIZE;
            left += BLOCK_SIZE_SQUARED * colsLeftBlockCount;  
        }
        left -= BLOCK_SIZE_SQUARED * rowsRightBlockCount * colsLeftBlockCount;
        left += BLOCK_SIZE_SQUARED;
        out += BLOCK_SIZE;
    }
}

void BM_Transpose2Mul(const Matrix* a, const Matrix* b, Matrix* c)
{
#if DEBUG
    if(a->cols != b->cols || a->rows != c->rows || b->rows != c->cols)
    {
        errx(1,"BM_Transpose2Mul() : Matrix dimensions do not match\n");
    }
#endif


    float* left = a->data;
    float* right = b->data;
    float* out = c->data;

    size_t rowsRightBlockCount = b->rows / BLOCK_SIZE;
    size_t colsRightBlockCount = b->cols / BLOCK_SIZE;
    size_t rowsLeftBlockCount = a->rows / BLOCK_SIZE;

    // The two loops are used to go through the blocks of the matrices
    for (size_t f = 0; f < rowsLeftBlockCount; f++)
    {
        right = b->data;
        for (size_t i = 0; i < colsRightBlockCount; i++) 
        {
            for (size_t j = 0; j < rowsRightBlockCount; j++)
            {
                //printf("right index %li\n",right - b->data);
                for (size_t ii = 0; ii < BLOCK_SIZE; ii++)
                {
                    for (size_t jj = 0; jj < BLOCK_SIZE; jj++)
                    {
                        float* res = (out + jj + ii * BLOCK_SIZE);
                        size_t kk = 0;
                        for (; kk < BLOCK_SIZE; kk++)
                        {
                            // Go to the next column in matrix A (because it's transposed) 
                            // and go to the next row in matrix B
                            //printf("right index %li\n",right - b->data + kk * BLOCK_SIZE + jj);
                            //printf("kk %li jj %li ii %li j %li i %li\n",kk,jj,ii,j,i);
                            *res += left[kk + ii * BLOCK_SIZE] * right[kk + jj * BLOCK_SIZE];  
                        }
                    }
                }
                right += BLOCK_SIZE_SQUARED * colsRightBlockCount;
                out += BLOCK_SIZE_SQUARED;
            }
            right -= BLOCK_SIZE_SQUARED * rowsRightBlockCount * colsRightBlockCount;
            right += BLOCK_SIZE_SQUARED;
            left += BLOCK_SIZE_SQUARED;
            out -= BLOCK_SIZE_SQUARED * rowsRightBlockCount; 
        }
        out += BLOCK_SIZE_SQUARED * rowsRightBlockCount;
    }
}

void BM_M_Transpose2Mul(const Matrix* a, const Matrix* b, Matrix* c)
{


#if DEBUG
    if(a->cols != b->cols || a->rows != c->effectiveRows || b->rows != c->effectiveCols || b->cols != 1)
    {
        errx(1,"BM_M_Transpose2Mul() : Matrix dimensions do not match\n");
    }
#endif


    float* left = a->data;
    float* right = b->data;
    float* out = c->data;

    size_t rowsRightBlockCount = c->cols / BLOCK_SIZE;
    size_t rowsLeftBlockCount = c->rows / BLOCK_SIZE;

    size_t missingOutRows = a->rows % BLOCK_SIZE;
    size_t missingOutCols = b->rows % BLOCK_SIZE;
    if(missingOutRows == 0)
    {
        missingOutRows = BLOCK_SIZE;
    }
    if(missingOutCols == 0)
    {
        missingOutCols = BLOCK_SIZE;
    }

    

    // The two loops are used to go through the blocks of the matrices
    for (size_t f = 0; f < rowsLeftBlockCount; f++)
    {
        size_t colsLoopCount = (f == rowsLeftBlockCount - 1) ? missingOutRows : BLOCK_SIZE;
        right = b->data;
        for (size_t j = 0; j < rowsRightBlockCount; j++)
        {
            //printf("right index %li\n",right - b->data);

            size_t rowsLoopCount = (j == rowsRightBlockCount - 1) ? missingOutCols : BLOCK_SIZE;

            for (size_t ii = 0; ii < colsLoopCount; ii++)
            {
                for (size_t jj = 0; jj < rowsLoopCount; jj++)
                {
                    float* res = (out + jj + ii * BLOCK_SIZE);
                    
                        // Go to the next column in matrix A (because it's transposed) 
                        // and go to the next row in matrix B
                    *res += left[ii] * right[jj];
                    
                }
            }
            right += BLOCK_SIZE;
            out += BLOCK_SIZE_SQUARED;
        }
        left += BLOCK_SIZE;
    }
}

void BM_M_SSE_Transpose2Mul(const Matrix* a, const Matrix* b, Matrix* c)
{
    #if DEBUG
    if(a->cols != b->cols || a->rows != c->effectiveRows || b->rows != c->effectiveCols || b->cols != 1)
    {
        errx(1,"BM_M_Transpose2Mul() : Matrix dimensions do not match\n");
    }
#endif


    float* left = a->data;
    float* right = b->data;
    float* out = c->data;

    size_t rowsRightBlockCount = c->cols / BLOCK_SIZE;
    size_t rowsLeftBlockCount = c->rows / BLOCK_SIZE;

    size_t missingOutRows = a->rows % BLOCK_SIZE;
    size_t missingOutCols = b->rows % BLOCK_SIZE;
    if(missingOutRows == 0)
    {
        missingOutRows = BLOCK_SIZE;
    }
    if(missingOutCols == 0)
    {
        missingOutCols = BLOCK_SIZE;
    }

    

    // The two loops are used to go through the blocks of the matrices
    for (size_t f = 0; f < rowsLeftBlockCount; f++)
    {
        right = b->data;
        for (size_t j = 0; j < rowsRightBlockCount; j++)
        {
            //printf("right index %li\n",right - b->data);

            size_t colsLoopCount = (f == rowsLeftBlockCount - 1) ? missingOutRows : BLOCK_SIZE;
            size_t rowsLoopCount = (j == rowsRightBlockCount - 1) ? missingOutCols : BLOCK_SIZE;

            for (size_t ii = 0; ii < colsLoopCount; ii++)
            {
                //sse loop
                size_t jj = 0;
#if SSE     
                __m128 leftVector = _mm_set1_ps(left[ii]);

                for (; jj + 4 <= rowsLoopCount; jj += 4)
                {
                    float* res = (out + jj + ii * BLOCK_SIZE);
                    __m128 resVector = _mm_load_ps(res);

                    // If 'left[ii]' is indeed a single value per iteration, you should broadcast it
                    // to all elements of a __m128 vector, rather than loading potentially unrelated values.
                     // Broadcast 'left[ii]' to all four elements of leftVector

                    __m128 rightVector = _mm_load_ps(right + jj);
                    __m128 mulVector = _mm_mul_ps(leftVector, rightVector);
                    resVector = _mm_add_ps(resVector, mulVector);
                    _mm_store_ps(res, resVector);
                }
                
#endif

                for (; jj < rowsLoopCount; jj++)
                {
                    float* res = (out + jj + ii * BLOCK_SIZE);
                    
                        // Go to the next column in matrix A (because it's transposed) 
                        // and go to the next row in matrix B
                    *res += left[ii] * right[jj];
                    
                }
            }
            right += BLOCK_SIZE;
            out += BLOCK_SIZE_SQUARED;
        }
        left += BLOCK_SIZE;
    }

}

void BM_M_AVX_Transpose2Mul(const Matrix* a, const Matrix* b, Matrix* c) {
#if DEBUG
    if (a->cols != b->cols || a->rows != c->effectiveRows || b->rows != c->effectiveCols || b->cols != 1) {
        M_Dim(a);
        M_Dim(b);
        M_Dim(c);
        errx(1, "BM_M_Transpose2Mul() : Matrix dimensions do not match\n");
    }
#endif

    float* __restrict left = a->data;
    float* __restrict right = b->data;
    float* __restrict out = c->data;

    const size_t rowsRightBlockCount = c->cols / BLOCK_SIZE;
    const size_t rowsLeftBlockCount = c->rows / BLOCK_SIZE;

    size_t missingOutRows = a->rows % BLOCK_SIZE;
    size_t missingOutCols = b->rows % BLOCK_SIZE;
    if (missingOutRows == 0) { missingOutRows = BLOCK_SIZE; }
    if (missingOutCols == 0) { missingOutCols = BLOCK_SIZE; }

#if SSE
    // Using aligned memory functions for better performance with AVX
    float* tempBuffer = (float*)_mm_malloc(sizeof(float) * BLOCK_SIZE * BLOCK_SIZE, 64);

    for (size_t f = 0; f < rowsLeftBlockCount; ++f) {
        size_t colsLoopCount = (f == rowsLeftBlockCount - 1) ? missingOutRows : BLOCK_SIZE;
        right = b->data;
        for (size_t j = 0; j < rowsRightBlockCount; ++j) {
            size_t rowsLoopCount = (j == rowsRightBlockCount - 1) ? missingOutCols : BLOCK_SIZE;

            memset(tempBuffer, 0, sizeof(float) * BLOCK_SIZE * BLOCK_SIZE);

            for (size_t ii = 0; ii < colsLoopCount; ++ii) {
                float* __restrict res = (tempBuffer + ii * BLOCK_SIZE);
                size_t jj = 0;

#if AVX     
                __m256 leftVector = _mm256_broadcast_ss(left + ii);

                for (; jj + 8 <= rowsLoopCount; jj += 8) {
                    __m256 rightVector = _mm256_load_ps(right + jj);
                    __m256 resVector = _mm256_load_ps(res + jj);
                    resVector = _mm256_fmadd_ps(leftVector, rightVector, resVector);
                    _mm256_store_ps(res + jj, resVector);
                }
#endif

                for (; jj < rowsLoopCount; ++jj) {
                    res[jj] += left[ii] * right[jj];
                }
            }

            // Combine the temporary results with the main output, rather than streaming them directly.
            for (size_t i = 0; i < BLOCK_SIZE_SQUARED; i += 8) {
                _mm256_store_ps(out + i, _mm256_add_ps(_mm256_load_ps(out + i), _mm256_load_ps(tempBuffer + i)));
            }

            right += BLOCK_SIZE;
            out += BLOCK_SIZE_SQUARED;
        }
        left += BLOCK_SIZE;
    }

    _mm_free(tempBuffer);
#endif
}

void BM_M_AVX_Transpose2MulLoop2(float *left, float *right, float *out, size_t ii)
{

    //sse loop
    float* res = (out + ii * BLOCK_SIZE);
#if AVX     
    __m256 leftVector = _mm256_broadcast_ss(left + ii);
    
    for (size_t jj = 0; jj < AVX_OPERATION_COUNT; jj++)
    {
        __m256 resVector = _mm256_load_ps(res + (jj*8));

        __m256 rightVector = _mm256_load_ps(right + (jj*8));
        resVector = _mm256_fmadd_ps(leftVector, rightVector, resVector);
        _mm256_store_ps(res + (jj*8), resVector);
    }
    
#endif

}

void BM_M_AVX_Transpose2MulLoop(float *left, float *right, float *out, size_t ii, size_t rowsLoopCount)
{

    //sse loop
    float* res = (out + ii * BLOCK_SIZE);
    size_t jj = 0;
#if AVX     
    __m256 leftVector = _mm256_broadcast_ss(left + ii);
    
    for (; jj + 8 <= rowsLoopCount; jj += 8)
    {
        __m256 resVector = _mm256_load_ps(res + jj);

        __m256 rightVector = _mm256_load_ps(right + jj);
        resVector = _mm256_fmadd_ps(leftVector, rightVector, resVector);
        _mm256_store_ps(res + jj, resVector);
    }
    
#endif

    for (; jj < rowsLoopCount; jj++)
    {
    
        res[jj] += left[ii] * right[jj];
        
    }
}

void BM_M_AVX_NCOND_Transpose2Mul(const Matrix* a, const Matrix* b, Matrix* c)
{
#if DEBUG
    if(a->cols != b->cols || a->rows != c->effectiveRows || b->rows != c->effectiveCols || b->cols != 1)
    {
        errx(1,"BM_M_Transpose2Mul() : Matrix dimensions do not match\n");
    }
#endif


    float* left = a->data;
    float* right = b->data;
    float* out = c->data;

    const size_t rowsRightBlockCount = c->cols / BLOCK_SIZE;
    const size_t rowsLeftBlockCount = c->rows / BLOCK_SIZE;

    size_t missingOutRows = a->rows % BLOCK_SIZE;
    size_t missingOutCols = b->rows % BLOCK_SIZE;
    if(missingOutRows == 0)
    {
        missingOutRows = BLOCK_SIZE;
    }
    if(missingOutCols == 0)
    {
        missingOutCols = BLOCK_SIZE;
    }

    

    // The two loops are used to go through the blocks of the matrices
    for (size_t f = 0; f < rowsLeftBlockCount; f++)
    {
        size_t colsLoopCount = ((f == rowsLeftBlockCount - 1) ? missingOutRows : BLOCK_SIZE)-1;
        right = b->data;
        for (size_t j = 0; j < rowsRightBlockCount; j++)
        {
            //printf("right index %li\n",right - b->data);

            for (size_t ii = 0; ii < colsLoopCount; ii++)
            {
                BM_M_AVX_Transpose2MulLoop2(left,right,out,ii);
            }
            BM_M_AVX_Transpose2MulLoop(left,right,out,colsLoopCount,missingOutCols);
            right += BLOCK_SIZE;
            out += BLOCK_SIZE_SQUARED;
        }
        left += BLOCK_SIZE;
    }
}



void BM_Print(const Matrix* a)
{
    printf("Matrix: %li x %li\n",a->rows,a->cols); 

    int squareBlockSize = BLOCK_SIZE * BLOCK_SIZE;
    for (size_t i = 0; i < a->rows; i++)
    {
        printf("[");
        for (size_t j = 0; j < a->cols; j++)
        {
            printf("%f",a->data[(j / BLOCK_SIZE) * squareBlockSize + j % BLOCK_SIZE + (i%BLOCK_SIZE) * BLOCK_SIZE + (i/BLOCK_SIZE) * BLOCK_SIZE * BLOCK_SIZE * (a->cols / BLOCK_SIZE)]);
            //std::cout << "print : " << (j / BLOCK_SIZE) * squareBlockSize + j % BLOCK_SIZE + (i%BLOCK_SIZE) * BLOCK_SIZE + (i/BLOCK_SIZE) * BLOCK_SIZE * BLOCK_SIZE * (matrix.GetCols() / BLOCK_SIZE) << "\n";
            if (j != a->cols - 1)
            {
                printf(" ");
            }
        }
        printf("]\n");
        
    }
}

int BM_M_Equals(const Matrix* blockMatrix, const Matrix* matrix)
{
    if(blockMatrix->effectiveRows != matrix->rows || blockMatrix->effectiveCols != matrix->cols)
    {
        return 0;
    }

    for (size_t i = 0; i < blockMatrix->effectiveRows; i++)
    {
        for (size_t j = 0; j < blockMatrix->effectiveCols; j++)
        {
            float value1 = blockMatrix->data[(j / BLOCK_SIZE) * BLOCK_SIZE_SQUARED + j % BLOCK_SIZE + (i%BLOCK_SIZE) * BLOCK_SIZE + (i/BLOCK_SIZE) * BLOCK_SIZE * BLOCK_SIZE * (blockMatrix->cols / BLOCK_SIZE)];
            float value2 = matrix->data[j + i * blockMatrix->effectiveCols];

            if(fabsf(value1 - value2) > 0.001)
            {
                return 0;
            }
        }
        
    }
    return 1;
}

Matrix* M_BM_Convert(const Matrix* a)
{
    Matrix* blockMatrix = BM_Create_2D(a->rows, a->cols);

    size_t missingRows = a->rows % BLOCK_SIZE;
    size_t missingCols = a->cols % BLOCK_SIZE;

    for (size_t i = 0; i < a->rows; i+=BLOCK_SIZE)
    {
        for (size_t j = 0; j < a->cols; j+=BLOCK_SIZE)
        {
            size_t blockRows = (i + BLOCK_SIZE <= a->rows) ? BLOCK_SIZE : missingRows;
            for (size_t ii = 0; ii < blockRows; ii++)
            {
                size_t blockCols = (j + BLOCK_SIZE <= a->cols) ? BLOCK_SIZE : missingCols;

                for (size_t jj = 0; jj < blockCols; jj++)
                {
                    blockMatrix->data[i * blockMatrix->cols + j * BLOCK_SIZE + jj + ii * BLOCK_SIZE] = a->data[(ii * a->cols + jj) + j + i * a->cols];
                }
            }
        }
        
    }
    
    return blockMatrix;
}

void BM_SetValue(Matrix* a, size_t index, float value)
{
    size_t rowsIndex = index / a->effectiveCols;
    size_t colsIndex = index % a->effectiveCols;
    size_t finalIndex = (rowsIndex % BLOCK_SIZE) * BLOCK_SIZE + (rowsIndex / BLOCK_SIZE) * BLOCK_SIZE_SQUARED * (a->cols / BLOCK_SIZE) + (colsIndex/BLOCK_SIZE) * BLOCK_SIZE_SQUARED + (colsIndex % BLOCK_SIZE);
    /*
    printf("rowsIndex %li colsIndex %li finalIndex %li\n",rowsIndex,colsIndex,finalIndex);
    */
    a->data[finalIndex] = value;
}


float BM_GetValue(const Matrix* a, size_t index)
{
    size_t rowsIndex = index / a->effectiveCols;
    size_t colsIndex = index % a->effectiveCols;
    size_t finalIndex = (rowsIndex % BLOCK_SIZE) * BLOCK_SIZE + (rowsIndex / BLOCK_SIZE) * BLOCK_SIZE_SQUARED * (a->cols / BLOCK_SIZE) + (colsIndex/BLOCK_SIZE) * BLOCK_SIZE_SQUARED + (colsIndex % BLOCK_SIZE);
    return a->data[finalIndex];
}

size_t SM_GET_BLOCK_SIZE()
{
    if(M_GetMatrixType() == 0)
        return 1;
    return BLOCK_SIZE;
}