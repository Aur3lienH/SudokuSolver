#pragma once
#include "matrix/Matrix.h"
#include "imageProcessing/Image.h"


//void AddDigit(stbtt_fontinfo fontInfo,Matrix* image, int digit, int x, int y, int width, int height);


void AddMissingDigits(Matrix* image, int** digits, int cellWidth, Color color);


