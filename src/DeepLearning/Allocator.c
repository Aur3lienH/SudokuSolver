#include "Allocator.h"
#include <stdlib.h>
#include <err.h>
#include <stdio.h>
float* startingPointer = NULL;
float* pointer = NULL;
size_t size = 0;

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


void A_Init(const size_t _size)
{
#if SSE
    pointer = (float*)_mm_malloc(_size,32);
#else
    pointer = (float*)malloc(_size);
#endif
    if(pointer == NULL)
    {
        errx(-1,"Out of memory");
    }
    startingPointer = pointer;
    size = _size;
}

void* M_Allocate(const size_t _size)
{
    if(size < _size + (pointer - startingPointer))
    {
        errx(-1,"Out of memory");
    }
    if(pointer == NULL)
    {
        errx(-1,"Allocator not initialized");
    }
    void* temp = pointer;
    pointer += _size;
    return temp;
}