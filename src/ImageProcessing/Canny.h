#pragma once
#include <SDL2/SDL.h>
#include "../DeepLearning/Matrix.h"


Matrix* canny(Matrix* input, float sigma);


Matrix* getSquare(Matrix* canny);