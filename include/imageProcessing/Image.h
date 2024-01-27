#pragma once
#include <stdlib.h>
#include "matrix/Matrix.h"



//Structure of image used for mobile
typedef struct ImageMobile
{
    size_t width;
    size_t height;
    size_t channels;
    unsigned char* pixels;
} ImageMobile;

#include "imageProcessing/stb_image_write.h"
#include "imageProcessing/stb_image.h"
#if 0
#define MOBILE 1
typedef ImageMobile Image;
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
typedef SDL_Surface Image;
#define MOBILE 0
#endif


Image* Image_Create(size_t width, size_t height, size_t channels);

Image* Image_Load(const char* path);

void Image_Save(Image* image, const char* path);

Image* MatrixToImage(Matrix* matrix);

Matrix* ImageToMatrix(Image* image);

void Image_Free(Image* image);

void M_SaveImage(const Matrix* matrix, const char* path);