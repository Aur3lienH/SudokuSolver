#pragma once
#include <stdlib.h>
#include "matrix/Matrix.h"
#if 1
#define MOBILE 1
typedef ImageMobile Image;
#else
#include <SDL/SDL_image.h>
typedef SDL_Surface Image;
#endif

//Structure of image used for mobile
typedef struct ImageMobile
{
    size_t width;
    size_t height;
    size_t channels;
    unsigned char* pixels;
} ImageMobile;


ImageMobile* Image_Create(size_t width, size_t height, size_t channels);

ImageMobile* Image_Load(const char* path);

Image* MatrixToImage(Matrix* matrix);

Matrix* ImageToMatrix(Image* image);

void Image_Free(ImageMobile* image);