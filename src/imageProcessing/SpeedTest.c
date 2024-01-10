#include "imageProcessing/Grayscale.h"
#include <time.h>


const size_t outputWidth = 256;


void TestSpeedDownscaleGrayscale()
{
    Matrix* downGrayscaled = M_Create_2D(outputWidth,outputWidth);
    for (size_t i = 1; i < 10; i++)
    {
        char* path = malloc(sizeof(char) * 100);
        snprintf(path,100,"images/examples/image_0%i",i);
        SDL_Surface* image = IMG_Load(path);
        time_t start = clock();
        GrayscaleToMatrix_C(image,downGrayscaled);
        printf("Grayscaling took %f seconds\n", (float)(clock() - start) / CLOCKS_PER_SEC);
    }
    
}