#pragma once
#include "matrix/Matrix.h"
#include <SDL2/SDL.h>

//Grayscale an image and store the result in res
void GrayscaleToMatrix_C(SDL_Surface* image, Matrix* res);

//Grayscale an image and return the result
Matrix* GrayscaleToMatrix(SDL_Surface* image);

//Convert a matrix to a SDL_Surface
SDL_Surface* MatrixToSurface(Matrix* matrix);

//Save a matrix as an image
void M_SaveImage(const Matrix* matrix, const char* path);