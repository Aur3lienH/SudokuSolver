#include "ImageProcessing.h"
#include "Preprocessing.h"
#include "Grayscale.h"
#include "Hough.h"
#include "../geometry/Square.h"


SDL_Surface* SudokuImgProcessing(SDL_Surface* image)
{
    Matrix* res = preprocess(image, 0.5);
    Square square = Hough(res);
    SDL_Surface* surface = MatrixToSurface(res);
    M_Free(res);
    return surface;
}