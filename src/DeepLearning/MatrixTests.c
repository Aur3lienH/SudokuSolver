#include "MatrixTests.h"
#include "Matrix.h"
#include "BlockMatrix.h"
#include <stdlib.h>
#include <time.h>
#include "RandomInit.h"
#include "Mnist.h"
#include "Allocator.h"
    
const size_t matrixSize = 100;
const size_t MAX_MATRIX_SIZE = 100;
const size_t TEST_COUNT = 10;


void TestFunction(int (*function)(char** name))
{
    char** name = (char**)malloc(sizeof(char*));
    int res = function(name);
    printf("%s : %s\n", *name, res ? "\033[1;32m[SUCCEED]\033[0m   " : "\033[1;31m[FAIL]\033[0m   ");
    free(name);
}

void MatrixOperationsTest(int (*function)(char** name, void (*M_Operation)(const Matrix*,const Matrix*,Matrix*), void (*BM_Operation)(const Matrix*,const Matrix*,Matrix*)))
{

    char** name = (char**)malloc(sizeof(char*));
    int res = function(name,M_Mul,BM_Mul);
    printf("%s : %s\n", *name, res ? " : BM_MUL \033[1;32m[SUCCEED]\033[0m   " : "\033[1;31m[FAIL]\033[0m   ");

    res = function(name,M_Transpose1Mul,BM_Transpose1Mul);
    printf("%s : %s\n", *name, res ? " : BM_Transpose1Mul \033[1;32m[SUCCEED]\033[0m   " : "\033[1;31m[FAIL]\033[0m   ");

    res = function(name,M_Transpose2Mul,BM_Transpose2Mul);
    printf("%s : %s\n", *name, res ? " : BM_Transpose2Mul \033[1;32m[SUCCEED]\033[0m   " : "\033[1;31m[FAIL]\033[0m   ");
    free(name);
}

int M_BM_MatrixTest(void (*M_Operation)(const Matrix*,const Matrix*,Matrix*), void (*BM_Operation)(const Matrix*,const Matrix*,Matrix*), size_t aRows, size_t aCols, size_t bRows, size_t bCols, size_t cRows, size_t cCols)
{
    Matrix* a = M_Create_2D(aRows, aCols);
    M_HeUniform(a);
    Matrix* b = M_Create_2D(bRows, bCols);
    M_HeUniform(b);
    Matrix* c = M_Create_2D(cRows, cCols);

    Matrix* abis = M_BM_Convert(a);
    Matrix* bbis = M_BM_Convert(b);
    Matrix* cbis = BM_Create_2D(cRows, cCols);

    M_Operation(a, b, c);
    BM_Operation(abis,bbis,cbis);

    int res = BM_M_Equals(cbis,c);

    M_Free(a);
    M_Free(b);
    M_Free(c);
    M_Free(abis);
    M_Free(bbis);
    M_Free(cbis);

    return res;
}


int TestTranspose1(char** name)
{
    *name = "Test Transpose 1";
    Matrix* a = M_Create_2D(matrixSize, matrixSize);
    M_HeUniform(a);
    Matrix* aT = M_Transpose(a);
    Matrix* b = M_Create_2D(matrixSize, matrixSize);
    M_HeUniform(b);
    Matrix* c = M_Create_2D(matrixSize, matrixSize);
    Matrix* cbis = M_Create_2D(matrixSize, matrixSize);

    M_Transpose1Mul(a, b, c);
    M_Mul(aT,b,cbis);
    
    int res = M_Equals(c, cbis);


    M_Free(a);
    M_Free(aT);
    M_Free(b);
    M_Free(c);
    M_Free(cbis);

    return res;
}

int TestTranspose2(char** name)
{
    *name = "Test Transpose 2";

    
    Matrix* a = M_Create_2D(matrixSize, matrixSize);
    M_HeUniform(a);
    Matrix* b = M_Create_2D(matrixSize, matrixSize);
    M_HeUniform(b);
    Matrix* bT = M_Transpose(b);


    Matrix* c = M_Create_2D(matrixSize, matrixSize);
    Matrix* cbis = M_Create_2D(matrixSize, matrixSize);

    M_Transpose2Mul(a, b, c);
    M_Mul(a,bT,cbis);

    int res = M_Equals(c, cbis);

    M_Free(a);
    M_Free(b);
    M_Free(bT);
    M_Free(c);
    M_Free(cbis);


    return res;
}


int TestRandomTranspose2(char** name)
{
    int res = 1;

    for (size_t i = 0; i < 100; i++)
    {
        const size_t randomMatrixSize1 = rand() % 100 + 1;
        const size_t randomMatrixSize2 = rand() % 100 + 1;
        *name = "Test BM_MulTranspose2 Random";
        Matrix* a = M_Create_2D(randomMatrixSize1, randomMatrixSize2);
        M_HeUniform(a);
        Matrix* b = M_Create_2D(randomMatrixSize1, randomMatrixSize2);
        M_HeUniform(b);
        Matrix* bT = M_Transpose(b);


        Matrix* c = M_Create_2D(randomMatrixSize1, randomMatrixSize1);
        Matrix* cbis = M_Create_2D(randomMatrixSize1, randomMatrixSize1);

        M_Transpose2Mul(a, b, c);

        M_Mul(a,bT,cbis);

        res = M_Equals(c, cbis) && res;

        M_Free(a);
        M_Free(b);
        M_Free(bT);
        M_Free(c);
        M_Free(cbis);
    }
    
    


    return res;
}

int TestOptimizedMatrix1(char** name)
{
    *name = "Test Optimized Matrix 1";
    Matrix* a = M_Create_2D(matrixSize, matrixSize);
    M_HeUniform(a);
    Matrix* b = M_BM_Convert(a);
    int res = BM_M_Equals(b,a);
    M_Free(a);
    M_Free(b);
    return res;
}

int TestOptimizedMul(char** name)
{
    int res = 1;
    
    // Performance measurement variables
    clock_t start, end;
    double cpu_time_used_normal, cpu_time_used_optimized;
    
    for (size_t i = 0; i < TEST_COUNT; i++)
    {
        const size_t size1 = rand() % MAX_MATRIX_SIZE + 1;
        const size_t size2 = rand() % MAX_MATRIX_SIZE + 1;
        const size_t size3 = rand() % MAX_MATRIX_SIZE + 1;

        Matrix* a = M_Create_2D(size1, size2);
        M_HeUniform(a);
        Matrix* b = M_Create_2D(size2, size3);
        M_HeUniform(b);
        Matrix* c = M_Create_2D(size1, size3);

        Matrix* abis = M_BM_Convert(a);
        Matrix* bbis = M_BM_Convert(b);
        Matrix* cbis = BM_Create_2D(size1, size3);

        // Measure performance of normal multiplication
        start = clock();
        M_Mul(a, b, c);
        end = clock();
        cpu_time_used_normal = ((double) (end - start)) / CLOCKS_PER_SEC;

        // Measure performance of block multiplication
        start = clock();
        BM_Mul(abis,bbis,cbis);
        end = clock();
        cpu_time_used_optimized = ((double) (end - start)) / CLOCKS_PER_SEC;

        // Print performance stats for this iteration
        //printf("Iteration %zu: Normal mul time: %f seconds, Optimized mul time: %f seconds\n", 
               //i, cpu_time_used_normal, cpu_time_used_optimized);

        res = BM_M_Equals(cbis,c) && res;

        M_Free(a);
        M_Free(b);
        M_Free(c);
        M_Free(abis);
        M_Free(bbis);
        M_Free(cbis);
    }

    *name = "Test Optimized Mul";

    return res;
}

int TestOptimizedTranspose1Mul(char** name)
{
    int res = 1;
    *name = "Test Optimized Transpose 1 Mul";
    for (size_t i = 0; i < TEST_COUNT; i++)
    {
        const size_t size1 = rand() % 5 + 1;
        const size_t size2 = rand() % 5 + 1;
        const size_t size3 = rand() % 5 + 1;

        Matrix* a = M_Create_2D(size1, size2);
        M_HeUniform(a);
        Matrix* b = M_Create_2D(size1, size3);
        M_HeUniform(b);
        Matrix* c = M_Create_2D(size2, size3);

        Matrix* abis = M_BM_Convert(a);
        Matrix* bbis = M_BM_Convert(b);
        Matrix* cbis = BM_Create_2D(size2, size3);

        M_Transpose1Mul(a, b, c);

        BM_Transpose1Mul(abis,bbis,cbis);


        res = BM_M_Equals(cbis,c) && res;

        M_Free(a);
        M_Free(b);
        M_Free(c);
        M_Free(abis);
        M_Free(bbis);
        M_Free(cbis);

    }
    return res;
    

    
}

int TestOptimizedTranspose2Mul(char** name)
{
    int res = 1;
    *name = "Test Optimized Transpose 2 Mul";
    for (size_t i = 0; i < TEST_COUNT; i++)
    {
        const size_t size1 = rand() % MAX_MATRIX_SIZE + 1;
        const size_t size2 = rand() % MAX_MATRIX_SIZE + 1;

        res = M_BM_MatrixTest(M_Transpose2Mul,BM_Transpose2Mul,
        size1,size2,
        size1,size2,
        size1,size1) && res;

    }
    


    return res;
}

int RandomSizeTestOptimizedMul(char** name, void (*M_Operation)(const Matrix*,const Matrix*,Matrix*), void (*BM_Operation)(const Matrix*,const Matrix*,Matrix*))
{
    int res = 1;
    *name = "Random size Test Optimized Mul";
    for (size_t i = 0; i < TEST_COUNT; i++)
    {

        const size_t randomMatrixSize = rand() % MAX_MATRIX_SIZE + 1;

        //printf("Random size Test Optimized Mul %zu\n", randomMatrixSize);
        Matrix* a = M_Create_2D(randomMatrixSize, randomMatrixSize);
        M_HeUniform(a);
        Matrix* b = M_Create_2D(randomMatrixSize, randomMatrixSize);
        M_HeUniform(b);
        Matrix* c = M_Create_2D(randomMatrixSize, randomMatrixSize);


        Matrix* abis = M_BM_Convert(a);
        Matrix* bbis = M_BM_Convert(b);
        Matrix* cbis = BM_Create_2D(randomMatrixSize, randomMatrixSize);
        

        M_Operation(a, b, c);


        BM_Operation(abis,bbis,cbis);

        res = BM_M_Equals(cbis,c) && res;


        M_Free(a);
        M_Free(b);
        M_Free(c);
        M_Free(abis);
        M_Free(bbis);
        M_Free(cbis);
    }
    return res;
}

int TestRandomOptimizedTranspose2Mul(char** name)
{
    *name = "Test Random1 X Random2 Size Optimized Transpose 2 Mul";
    int res = 1;

    for (size_t i = 0; i < TEST_COUNT; i++)
    {
        const size_t randomMatrixSize1 = rand() % MAX_MATRIX_SIZE + 1;
        const size_t randomMatrixSize2 = rand() % MAX_MATRIX_SIZE + 1;
        const size_t randomMatrixSize3 = rand() % MAX_MATRIX_SIZE + 1;

        res = M_BM_MatrixTest(M_Transpose2Mul,BM_Transpose2Mul,
        randomMatrixSize1,randomMatrixSize2,
        randomMatrixSize3,randomMatrixSize2,
        randomMatrixSize1,randomMatrixSize3) && res;
    }
    
    
    return res;
}

void TestPerformanceBM_Mul()
{
    const size_t size = 1024;
    const size_t size2 = 1024;
    Matrix* a = M_Create_2D(size, size2);
    M_HeUniform(a);
    Matrix* b = M_Create_2D(size2, size);
    M_HeUniform(b);
    Matrix* c = M_Create_2D(size, size);

    Matrix* abis = M_BM_Convert(a);
    Matrix* bbis = M_BM_Convert(b);
    Matrix* cbis = BM_Create_2D(size, size);

    const size_t count = 1;
    const size_t count2 = 1;

    const clock_t begin_time = clock();
    for (size_t i = 0; i < count; i++)
    {
        M_Mul(a, b, c);
    }
    printf("M_Mul %f\n", (float)(clock() - begin_time) / CLOCKS_PER_SEC);

    const clock_t begin_time2 = clock();
    for (size_t i = 0; i < count2; i++)
    {
        BM_Mul(abis, bbis, cbis);
    }
    printf("BM_Mul %f\n", (float)(clock() - begin_time2) / CLOCKS_PER_SEC);

    printf("BM_Mul is %f times faster\n", (float)(clock() - begin_time) / (float)(clock() - begin_time2));

    M_Free(a);
    M_Free(b);
    M_Free(c);
    M_Free(abis);
    M_Free(bbis);
    M_Free(cbis);
}

int TestM_BM_Mul(char** name)
{
    *name = "Test M_BM_Mul";
    int res = 1;
    for (size_t i = 0; i < TEST_COUNT; i++)
    {
        size_t randomMatrixSize1 = rand() % MAX_MATRIX_SIZE + 1;
        size_t randomMatrixSize2 = rand() % MAX_MATRIX_SIZE + 1;

        Matrix* a = M_Create_2D(randomMatrixSize1, randomMatrixSize2);
        M_HeUniform(a);
        Matrix* b = M_Create_2D(randomMatrixSize2, 1);
        M_HeUniform(b);
        Matrix* c = M_Create_2D(randomMatrixSize1, 1);

        Matrix* abis = M_BM_Convert(a);
        Matrix* bbis = M_CopyI(b);
        Matrix* cbis = M_Create_2D(randomMatrixSize1, 1);

        M_Mul(a, b, c);
        
        BM_M_Mul(abis, bbis, cbis);

        res = M_Equals(c, cbis) && res;


        M_Free(a);
        M_Free(b);
        M_Free(c);
        M_Free(abis);   
        M_Free(bbis);
        M_Free(cbis);
    }

    return res;
    
}

int TestBM_M_Transpose1(char** name)
{
    *name = "Test BM_M_Transpose1";
    int res = 1;

    for (size_t i = 0; i < TEST_COUNT; i++)
    {
        size_t randomMatrixSize1 = rand() % MAX_MATRIX_SIZE + 1;
        size_t randomMatrixSize2 = rand() % MAX_MATRIX_SIZE + 1;

        Matrix* a = M_Create_2D(randomMatrixSize1, randomMatrixSize2);
        M_HeUniform(a);
        Matrix* b = M_Create_2D(randomMatrixSize1, 1);
        M_HeUniform(b);
        Matrix* c = M_Create_2D(randomMatrixSize2, 1);

        Matrix* abis = M_BM_Convert(a);
        Matrix* bbis = M_CopyI(b);
        Matrix* cbis = M_CopyI(c);

        M_Transpose1Mul(a, b, c);
        
        BM_M_Transpose1Mul(abis, bbis, cbis);


        res = M_Equals(cbis,c) && res;


        M_Free(a);
        M_Free(b);
        M_Free(c);
        M_Free(abis);   
        M_Free(bbis);
        M_Free(cbis);
    }

    return res;
}

int TestBM_M_SSE_Transpose1(char** name)
{
    *name = "Test BM_M_SSE_Transpose1";
    int res = 1;

    for (size_t i = 0; i < TEST_COUNT; i++)
    {
        size_t randomMatrixSize1 = rand() % MAX_MATRIX_SIZE + 1;
        size_t randomMatrixSize2 = rand() % MAX_MATRIX_SIZE + 1;

        Matrix* a = M_Create_2D(randomMatrixSize1, randomMatrixSize2);
        M_HeUniform(a);
        Matrix* b = M_Create_2D(randomMatrixSize1, 1);
        M_HeUniform(b);
        Matrix* c = M_Create_2D(randomMatrixSize2, 1);

        Matrix* abis = M_BM_Convert(a);
        Matrix* bbis = M_CopyI(b);
        Matrix* cbis = M_CopyI(c);

        M_Transpose1Mul(a, b, c);
        
        BM_M_SSE_Transpose1Mul(abis, bbis, cbis);


        res = M_Equals(cbis,c) && res;


        M_Free(a);
        M_Free(b);
        M_Free(c);
        M_Free(abis);   
        M_Free(bbis);
        M_Free(cbis);
    }

    return res;
}

int TestBM_M_AVX_Transpose1(char** name)
{
    *name = "Test BM_M_AVX_Transpose1";
    int res = 1;

    for (size_t i = 0; i < TEST_COUNT; i++)
    {
        size_t randomMatrixSize1 = rand() % MAX_MATRIX_SIZE + 1;
        size_t randomMatrixSize2 = rand() % MAX_MATRIX_SIZE + 1;

        Matrix* a = M_Create_2D(randomMatrixSize1, randomMatrixSize2);
        M_HeUniform(a);
        Matrix* b = M_Create_2D(randomMatrixSize1, 1);
        M_HeUniform(b);
        Matrix* c = M_Create_2D(randomMatrixSize2, 1);

        Matrix* abis = M_BM_Convert(a);
        Matrix* bbis = M_CopyI(b);
        Matrix* cbis = M_CopyI(c);

        M_Transpose1Mul(a, b, c);
        
        BM_M_AVX_Transpose1Mul(abis, bbis, cbis);


        res = M_Equals(cbis,c) && res;


        M_Free(a);
        M_Free(b);
        M_Free(c);
        M_Free(abis);   
        M_Free(bbis);
        M_Free(cbis);
    }

    return res;
}

int TestBM_M_Transpose2(char** name)
{
    *name = "Test BM_M_Transpose2";
    int res = 1;

    for (size_t i = 0; i < TEST_COUNT; i++)
    {
        size_t randomMatrixSize1 = rand() % MAX_MATRIX_SIZE + 1;
        size_t randomMatrixSize2 = rand() % MAX_MATRIX_SIZE + 1;

        Matrix* a = M_Create_2D(randomMatrixSize2, 1);
        M_HeUniform(a);
        Matrix* b = M_Create_2D(randomMatrixSize1, 1);
        M_HeUniform(b);
        Matrix* c = M_Create_2D(randomMatrixSize2, randomMatrixSize1);

        Matrix* abis = M_CopyI(a);
        Matrix* bbis = M_CopyI(b);
        Matrix* cbis = M_BM_Convert(c);

        M_Transpose2Mul(a, b, c);
        
        BM_M_Transpose2Mul(abis, bbis, cbis);


        res = BM_M_Equals(cbis,c) && res;

        M_Free(a);
        M_Free(b);
        M_Free(c);
        M_Free(abis);   
        M_Free(bbis);
        M_Free(cbis);
    }

    return res;
}

int TestBM_M_SSE_Transpose2(char** name)
{
    *name = "Test BM_M_SSE_Transpose2";
    int res = 1;

    for (size_t i = 0; i < TEST_COUNT; i++)
    {
        size_t randomMatrixSize1 = rand() % MAX_MATRIX_SIZE + 1;
        size_t randomMatrixSize2 = rand() % MAX_MATRIX_SIZE + 1;

        Matrix* a = M_Create_2D(randomMatrixSize2, 1);
        M_HeUniform(a);
        Matrix* b = M_Create_2D(randomMatrixSize1, 1);
        M_HeUniform(b);
        Matrix* c = M_Create_2D(randomMatrixSize2, randomMatrixSize1);

        Matrix* abis = M_CopyI(a);
        Matrix* bbis = M_CopyI(b);
        Matrix* cbis = M_BM_Convert(c);

        M_Transpose2Mul(a, b, c);
        
        BM_M_SSE_Transpose2Mul(abis, bbis, cbis);


        res = BM_M_Equals(cbis,c) && res;

        M_Free(a);
        M_Free(b);
        M_Free(c);
        M_Free(abis);   
        M_Free(bbis);
        M_Free(cbis);
    }

    return res;
}

int TestBM_M_AVX_Transpose2(char** name)
{
    *name = "Test BM_M_AVX_Transpose2";
    int res = 1;

    for (size_t i = 0; i < TEST_COUNT; i++)
    {
        size_t randomMatrixSize1 = rand() % MAX_MATRIX_SIZE + 1;
        size_t randomMatrixSize2 = rand() % MAX_MATRIX_SIZE + 1;

        Matrix* a = M_Create_2D(randomMatrixSize2, 1);
        M_HeUniform(a);
        Matrix* b = M_Create_2D(randomMatrixSize1, 1);
        M_HeUniform(b);
        Matrix* c = M_Create_2D(randomMatrixSize2, randomMatrixSize1);

        Matrix* abis = M_CopyI(a);
        Matrix* bbis = M_CopyI(b);
        Matrix* cbis = M_BM_Convert(c);

        M_Transpose2Mul(a, b, c);
        
        BM_M_AVX_Transpose2Mul(abis, bbis, cbis);


        res = BM_M_Equals(cbis,c) && res;

        M_Free(a);
        M_Free(b);
        M_Free(c);
        M_Free(abis);   
        M_Free(bbis);
        M_Free(cbis);
    }

    return res;
}

int TestBM_M_AVX_NCOND_Transpose2(char** name)
{
    *name = "Test BM_M_AVX_NCOND_Transpose2";
    int res = 1;

    for (size_t i = 0; i < TEST_COUNT; i++)
    {
        size_t randomMatrixSize1 = rand() % MAX_MATRIX_SIZE + 1;
        size_t randomMatrixSize2 = rand() % MAX_MATRIX_SIZE + 1;

        Matrix* a = M_Create_2D(randomMatrixSize2, 1);
        M_HeUniform(a);
        Matrix* b = M_Create_2D(randomMatrixSize1, 1);
        M_HeUniform(b);
        Matrix* c = M_Create_2D(randomMatrixSize2, randomMatrixSize1);

        Matrix* abis = M_CopyI(a);
        Matrix* bbis = M_CopyI(b);
        Matrix* cbis = M_BM_Convert(c);

        M_Transpose2Mul(a, b, c);
        
        BM_M_AVX_NCOND_Transpose2Mul(abis, bbis, cbis);


        res = BM_M_Equals(cbis,c) && res;

        M_Free(a);
        M_Free(b);
        M_Free(c);
        M_Free(abis);   
        M_Free(bbis);
        M_Free(cbis);
    }

    return res;
}

int TestBM_M_SSE_Mul(char** name)
{
    *name = "Test M_BM_SSE_Mul";
    int res = 1;
    for (size_t i = 0; i < TEST_COUNT; i++)
    {
        size_t randomMatrixSize1 = rand() % MAX_MATRIX_SIZE + SM_GET_BLOCK_SIZE();
        size_t randomMatrixSize2 = rand() % MAX_MATRIX_SIZE + SM_GET_BLOCK_SIZE();

        Matrix* a = M_Create_2D(randomMatrixSize1, randomMatrixSize2);
        M_HeUniform(a);
        Matrix* b = M_Create_2D(randomMatrixSize2, 1);
        M_HeUniform(b);
        Matrix* c = M_Create_2D(randomMatrixSize1, 1);

        Matrix* abis = M_BM_Convert(a);
        Matrix* bbis = M_CopyI(b);
        Matrix* cbis = M_Create_2D(randomMatrixSize1, 1);

        M_Mul(a, b, c);
        
        BM_M_SSE_Mul(abis, bbis, cbis);

        res = M_Equals(c, cbis) && res;


        M_Free(a);
        M_Free(b);
        M_Free(c);
        M_Free(abis);   
        M_Free(bbis);
        M_Free(cbis);
    }

    return res;
}

int TestBM_M_AVX_Mul(char** name)
{
    *name = "Test M_BM_AVX_Mul";
    int res = 1;
    for (size_t i = 0; i < TEST_COUNT; i++)
    {
        size_t randomMatrixSize1 = rand() % MAX_MATRIX_SIZE + 1;
        size_t randomMatrixSize2 = rand() % MAX_MATRIX_SIZE + 1;

        Matrix* a = M_Create_2D(randomMatrixSize1, randomMatrixSize2);
        M_HeUniform(a);
        Matrix* b = M_Create_2D(randomMatrixSize2, 1);
        M_HeUniform(b);
        Matrix* c = M_Create_2D(randomMatrixSize1, 1);

        Matrix* abis = M_BM_Convert(a);
        Matrix* bbis = M_CopyI(b);
        Matrix* cbis = M_Create_2D(randomMatrixSize1, 1);

        M_Mul(a, b, c);
        
        BM_M_AVX_Mul(abis, bbis, cbis);

        res = M_Equals(c, cbis) && res;


        M_Free(a);
        M_Free(b);
        M_Free(c);
        M_Free(abis);   
        M_Free(bbis);
        M_Free(cbis);
    }

    return res;
}

int TestBM_M_SSE_NCOND_Mul(char** name)
{
    *name = "Test M_BM_SSE_NCOND_Mul";
    int res = 1;
    for (size_t i = 0; i < TEST_COUNT; i++)
    {
        size_t randomMatrixSize1 = 2000;
        size_t randomMatrixSize2 = 2000;

        Matrix* a = M_Create_2D(randomMatrixSize1, randomMatrixSize2);
        M_HeUniform(a);
        Matrix* b = M_Create_2D(randomMatrixSize2, 1);
        M_HeUniform(b);
        Matrix* c = M_Create_2D(randomMatrixSize1, 1);

        Matrix* abis = M_BM_Convert(a);
        Matrix* bbis = M_CopyI(b);
        Matrix* cbis = M_Create_2D(randomMatrixSize1, 1);

        M_Mul(a, b, c);
        
        BM_M_SSE_NCOND_Mul(abis, bbis, cbis);

        res = M_Equals(c, cbis) && res;


        M_Free(a);
        M_Free(b);
        M_Free(c);
        M_Free(abis);   
        M_Free(bbis);
        M_Free(cbis);
    }

    return res;
}
int TestBM_M_AVX_NCOND_Mul(char** name)
{
    *name = "Test M_BM_AVX_NCOND_Mul";
    int res = 1;
    for (size_t i = 0; i < TEST_COUNT; i++)
    {
        size_t randomMatrixSize1 = rand() % MAX_MATRIX_SIZE + 1;
        size_t randomMatrixSize2 = rand() % MAX_MATRIX_SIZE + 1;

        Matrix* a = M_Create_2D(randomMatrixSize1, randomMatrixSize2);
        //M_HeUniform(a);
        Matrix* b = M_Create_2D(randomMatrixSize2, 1);
        //M_HeUniform(b);
        Matrix* c = M_Create_2D(randomMatrixSize1, 1);

        Matrix* abis = M_BM_Convert(a);
        Matrix* bbis = M_CopyI(b);
        Matrix* cbis = M_Create_2D(randomMatrixSize1, 1);

        //M_Mul(a, b, c);
        
        BM_M_AVX_NCOND_Mul(abis, bbis, cbis);

        res = M_Equals(c, cbis) && res;


        M_Free(a);
        M_Free(b);
        M_Free(c);
        M_Free(abis);   
        M_Free(bbis);
        M_Free(cbis);
    }

    return res;
}


void BM_M_Mul_PerformanceTest()
{
    size_t size = 4096;
    size_t size2 = 4096;
    printf("size %zu\n", size);
    Matrix* a = M_Create_2D(size, size2);
    M_HeUniform(a);
    Matrix* b = M_Create_2D(size2, 1);
    M_HeUniform(b);
    Matrix* c = M_Create_2D(size, 1);

    Matrix* abis = M_BM_Convert(a);
    Matrix* bbis = M_CopyI(b);
    Matrix* cbis = M_Create_2D(size, 1);


    const size_t count = 10;

    const clock_t begin_time = clock();
    for (size_t i = 0; i < count; i++)
    {
        M_Mul(a, b, c);
    }
    float duration1 = (float)(clock() - begin_time) / CLOCKS_PER_SEC;
    printf("M_Mul %f\n", duration1);

    const clock_t begin_time2 = clock();
    for (size_t i = 0; i < count; i++)
    {
        BM_M_Mul(abis, bbis, cbis);
    }
    float duration2 = (float)(clock() - begin_time2) / CLOCKS_PER_SEC;
    printf("BM_M_Mul %f\n", duration2);

    const clock_t begin_time3 = clock();
    for (size_t i = 0; i < count; i++)
    {
        BM_M_SSE_Mul(abis, bbis, cbis);
    }
    float duration3 = (float)(clock() - begin_time3) / CLOCKS_PER_SEC;
    printf("BM_M_SSE_Mul %f\n", duration3);

    const clock_t begin_time4 = clock();
    for (size_t i = 0; i < count; i++)
    {
        BM_M_AVX_Mul(abis, bbis, cbis);
    }
    float duration4 = (float)(clock() - begin_time4) / CLOCKS_PER_SEC;
    printf("BM_M_AVX_Mul %f\n", duration4);

    const clock_t begin_time5 = clock();
    for (size_t i = 0; i < count; i++)
    {
        BM_M_SSE_NCOND_Mul(abis, bbis, cbis);
    }
    float duration5 = (float)(clock() - begin_time5) / CLOCKS_PER_SEC;
    printf("BM_M_SSE_NCOND_Mul %f\n", duration5);

    const clock_t begin_time6 = clock();
    for (size_t i = 0; i < count; i++)
    {
        BM_M_AVX_NCOND_Mul(abis, bbis, cbis);
    }
    float duration6 = (float)(clock() - begin_time6) / CLOCKS_PER_SEC;
    printf("BM_M_AVX_NCOND_Mul %f\n", duration6);

    printf("\n");
    

    
    
    printf("BM_M_Mul is %f times faster\n", duration1 / (duration2 +1e-10));
    printf("BM_M_SSE_Mul is %f times faster\n", duration1 / (duration3+1e-10));
    printf("BM_M_AVX_Mul is %f times faster\n", duration1 / (duration4+1e-10));
    printf("BM_M_SSE_NCOND_Mul is %f times faster\n", duration1 / (duration5+1e-10));
    printf("BM_M_AVX_NCOND_Mul is %f times faster\n", duration1 / (duration6+1e-10));
    printf("\n");

    M_Free(a);
    M_Free(b);
    M_Free(c);
    M_Free(abis);
    M_Free(bbis);
    M_Free(cbis);

    

}

void BM_M_Transpose2Mul_PerformanceTest()
{
    size_t size = 4096;
    size_t size2 = 4096;
    printf("size %zu\n", size);
    Matrix* a = M_Create_2D(size, 1);
    M_HeUniform(a);
    Matrix* b = M_Create_2D(size2, 1);
    M_HeUniform(b);
    Matrix* c = M_Create_2D(size, size2);

    Matrix* abis = M_BM_Convert(a);
    Matrix* bbis = M_CopyI(b);
    Matrix* cbis = M_CopyI(c);


    const size_t count = 20;

    const clock_t begin_time = clock();
    for (size_t i = 0; i < count; i++)
    {
        M_Transpose2Mul(a, b, c);
    }
    float duration1 = (float)(clock() - begin_time) / CLOCKS_PER_SEC;
    printf("M_Transpose2Mul %f\n", duration1);

    const clock_t begin_time2 = clock();
    for (size_t i = 0; i < count; i++)
    {
        BM_M_Transpose2Mul(abis, bbis, cbis);
    }
    float duration2 = (float)(clock() - begin_time2) / CLOCKS_PER_SEC;
    printf("BM_M_Transpose2MulMul %f\n", duration2);

    const clock_t begin_time3 = clock();
    for (size_t i = 0; i < count; i++)
    {
        BM_M_SSE_Transpose2Mul(abis, bbis, cbis);
    }
    float duration3 = (float)(clock() - begin_time3) / CLOCKS_PER_SEC;
    printf("BM_M_SSE_Transpose2MulMul %f\n", duration3);

    const clock_t begin_time4 = clock();
    for (size_t i = 0; i < count; i++)
    {
        BM_M_AVX_Transpose2Mul(abis, bbis, cbis);
    }
    float duration4 = (float)(clock() - begin_time4) / CLOCKS_PER_SEC;
    printf("BM_M_AVX_Transpose2Mul %f\n", duration4);

    const clock_t begin_time5 = clock();
    for (size_t i = 0; i < count; i++)
    {
        BM_M_AVX_NCOND_Transpose2Mul(abis, bbis, cbis);
    }
    float duration5 = (float)(clock() - begin_time5) / CLOCKS_PER_SEC;
    printf("BM_M_SSE_NCOND_Transpose2Mul %f\n", duration5);


    printf("\n");
    

    
    
    printf("BM_M_Mul is %f times faster\n", duration1 / (duration2 +1e-10));
    printf("BM_M_SSE_Mul is %f times faster\n", duration1 / (duration3+1e-10));
    printf("BM_M_AVX_Mul is %f times faster\n", duration1 / (duration4+1e-10));
    printf("BM_M_SSE_NCOND_Mul is %f times faster\n", duration1 / (duration5+1e-10));
    printf("\n");

    M_Free(a);
    M_Free(b);
    M_Free(c);
    M_Free(abis);
    M_Free(bbis);
    M_Free(cbis);

    

}


void BM_M_AVX_Mul_PerformanceTest()
{
    for (size_t i = 0; i < 100; i++)
    {
        size_t size = i * 100 + 1;
        size_t size2 = i * 100 +1;
        Matrix* a = M_Create_2D(size, size2);
        M_HeUniform(a);
        Matrix* b = M_Create_2D(size2, 1);
        M_HeUniform(b);
        Matrix* c = M_Create_2D(size, 1);

        Matrix* abis = M_BM_Convert(a);
        Matrix* bbis = M_CopyI(b);
        Matrix* cbis = M_Create_2D(size, 1);

        const size_t count = 50;

        const clock_t begin_time = clock();
        for (size_t i = 0; i < count; i++)
        {
            M_Mul(a, b, c);
        }
        float duration1 = (float)(clock() - begin_time) / CLOCKS_PER_SEC;
        printf("M_Mul %f\n", duration1);

        const clock_t begin_time2 = clock();
        for (size_t i = 0; i < count; i++)
        {
            BM_M_Mul(abis, bbis, cbis);
        }
        printf("BM_M_Mul %f\n", (float)(clock() - begin_time2) / CLOCKS_PER_SEC);


        Matrix* abisbis = M_BM_Convert(a);
        const clock_t begin_time3 = clock();
        for (size_t i = 0; i < count; i++)
        {
            BM_M_AVX_Mul(abisbis, bbis, cbis);
        }
        float duration2 = (float)(clock() - begin_time3) / CLOCKS_PER_SEC;
        int success = M_Equals(cbis,c);
        //printf("BM_M_AVX_Mul %f\n", (float)(clock() - begin_time3) / CLOCKS_PER_SEC);
        printf("M_M_AVX_Mul %f\n", duration2);
        printf("BM_M_AVX_Mul is %f times faster success = %i\n", duration1 / duration2,success);
    }
    
    
    
    
    /*
    const clock_t begin_time3 = clock();
    for (size_t i = 0; i < count; i++)
    {
        BM_M_AVX_Mul(abis, bbis, cbis);
    }
    printf("BM_M_AVX_Mul %f\n", (float)(clock() - begin_time3) / CLOCKS_PER_SEC);
    

    printf("BM_M_Mul is %f times faster\n", (float)(clock() - begin_time) / (float)(clock() - begin_time2));
    */

}







void IndexTesting()
{
    Matrix* a = BM_Create_2D(5,5);
    for (size_t i = 0; i < 25; i++)
    {
        SM_SetValue(a, i, 1);
    }
}

int DatasetLoadingTest(char** name)
{
    *name = "Dataset Loading Test";
    return 1;
}

void M_Test()
{
    //TestFunction(TestTranspose1);
    TestFunction(TestTranspose2);
    TestFunction(TestOptimizedMatrix1);
    TestFunction(TestOptimizedMul);
    TestFunction(TestOptimizedTranspose1Mul);
    TestFunction(TestOptimizedTranspose2Mul);
    TestFunction(TestRandomTranspose2);
    MatrixOperationsTest(RandomSizeTestOptimizedMul);
    TestFunction(TestRandomOptimizedTranspose2Mul);
    
    TestFunction(TestM_BM_Mul);
    TestFunction(TestBM_M_Transpose1);
    TestFunction(TestBM_M_Transpose2);
    TestFunction(TestBM_M_SSE_Transpose1);
    TestFunction(TestBM_M_SSE_Transpose2);
    TestFunction(TestBM_M_AVX_Transpose1);
    TestFunction(TestBM_M_AVX_Transpose2);
    //TestFunction(TestBM_M_AVX_NCOND_Transpose2);
    
    TestFunction(TestBM_M_SSE_NCOND_Mul);
    TestFunction(TestBM_M_AVX_NCOND_Mul);
    

    
    //TestPerformanceBM_Mul();
    //TestFunction(TestBM_M_AVX_NCOND_Mul);
    BM_M_Mul_PerformanceTest();
    BM_M_Transpose2Mul_PerformanceTest();
}

void M_AVX_Test()
{
    TestFunction(TestBM_M_AVX_Mul);
    BM_M_AVX_Mul_PerformanceTest();
}

