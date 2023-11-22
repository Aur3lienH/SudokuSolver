#pragma once
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include "./deepLearning/Matrix.h"


Matrix* SurfaceToDigit(SDL_Surface* surface, int* isBlank);

SDL_Surface* Load(char* path);

Matrix* SurfaceToMatrix(SDL_Surface* surface);

Matrix* Downsize(const Matrix* input, size_t width, size_t height);


