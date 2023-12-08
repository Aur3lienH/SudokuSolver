#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Grayscale.h"
#include "Canny.h"
#include "Hough.h"
#include <time.h>
#include <SDL2/SDL.h>
#include "../geometry/Square.h"
#include "../geometry/Point.h"
#include "../Downscale.h"
#include "../deepLearning/ImageProcessing.h"
#include <math.h>


Matrix* resize(const Matrix* input, size_t downSizeWidth)
{
	size_t maximum = input->cols > input->rows ? input->cols : input->rows;
	float ratio = (float)downSizeWidth / (float)maximum;
	return DownScale(input,ratio);
}

Matrix* PreprocessToCanny(SDL_Surface* input, size_t downSizeWidth)
{
	Matrix* grayscaled = GrayscaleToMatrix(input);
	Matrix* resized = resize(grayscaled, downSizeWidth);
	Matrix* canny = Canny(resized, 1);
	M_Free(grayscaled);
	M_Free(resized);
	return canny;
}




Matrix* PreprocessSquare(Matrix* input)
{
	
}