#pragma once
#include <stdlib.h>
#include "matrix/Matrix.h"



//Structure of image used for mobile
typedef struct ImageMobile
{
    int width;
    int height;
    int channels;
    unsigned char* pixels;
} ImageMobile;

typedef struct Color
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Color;

#if 1
#define MOBILE 1
typedef ImageMobile Image;
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
typedef SDL_Surface Image;
#define MOBILE 0
#endif


Color Color_Create(unsigned char r, unsigned char g, unsigned char b);

Image* Image_Create(size_t width, size_t height, size_t channels);

Image* Image_Load(const char* path);

void Image_Save(Image* image, const char* path);

Image* MatrixToImage(Matrix* matrix);

Matrix* ImageToMatrix(Image* image);

void Image_Free(Image* image);

void M_SaveImage(const Matrix* matrix, const char* path);

Matrix* ImageTo3DMatrix(Image* image);

Image* Matrix3DToImage(Matrix* matrix);

void M_SaveImage3D(const Matrix* matrix, const char* path);
