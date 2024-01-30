#include "imageProcessing/SdlConverter.h"
#include <time.h>

#ifndef MOBILE
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


const size_t outputWidth = 256;


void TestSpeedDownscaleGrayscale()
{
    Matrix* downGrayscaled = M_Create_2D(outputWidth,outputWidth);
    for (size_t i = 1; i < 10; i++)
    {
        printf("Test %i\n",i);
        char* path = malloc(sizeof(char) * 100);
        snprintf(path,100,"images/examples/image_0%i.jpeg",i);
        printf("Loading image %s\n",path);  
        SDL_Surface* image = IMG_Load(path);
        time_t start = clock();
        DownGrayscaleToMatrix_C(image,downGrayscaled);
        time_t end = clock();
        printf("Grayscaling took %f seconds\n", (float)(end - start) / CLOCKS_PER_SEC);
        M_SaveImage(downGrayscaled,"images/export/downGrayscaled.jpg");
        free(path);

    }
}

#endif