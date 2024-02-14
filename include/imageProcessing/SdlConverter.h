#pragma once
#include "matrix/Matrix.h"
#include "imageProcessing/Image.h"
#ifndef __ANDROID__

#include <SDL2/SDL.h>


//Downscale an image and convert it to a matrix, store it in res
void DownGrayscaleToMatrix_C(SDL_Surface* image, Matrix* res);

//Downscale an image and convert it to a matrix, return the result
Matrix* DownGrayscaleToMatrix(SDL_Surface* image, size_t width);

//Grayscale an image and store the result in res
void GrayscaleToMatrix_C(SDL_Surface* image, Matrix* res);

//Grayscale an image and return the result
Matrix* GrayscaleToMatrix(SDL_Surface* image);

//Convert a matrix to a SDL_Surface
SDL_Surface* MatrixToSurface(const Matrix* matrix);

Matrix* SurfaceTo3DImage(SDL_Surface* surface);

#endif