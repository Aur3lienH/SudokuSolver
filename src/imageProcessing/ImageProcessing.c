#include "ImageProcessing.h"
#include "Preprocessing.h"
#include "Grayscale.h"
#include "Hough.h"
#include "SquareDetection.h"
#include "../geometry/Square.h"


SDL_Surface* SudokuImgProcessing(SDL_Surface* image)
{
    Matrix* res = preprocess(image, 0.5);
    Square square = GetSquareWithContour(res);
    SDL_Surface* surface = MatrixToSurface(res);
    M_Free(res);
    P_DrawSDL(surface,&square.points[0],0xFF0000);
    P_DrawSDL(surface,&square.points[1],0xFF0000);
    P_DrawSDL(surface,&square.points[2],0xFF0000);
    P_DrawSDL(surface,&square.points[3],0xFF0000);
    return surface;
}