#pragma once
#include <SDL2/SDL.h>
#include "../deepLearning/Matrix.h"


Matrix* canny(Matrix* input, float sigma);


Matrix* getSquare(Matrix* canny);