#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Grayscale.h"
#include "Canny.h"
#include <time.h>
#include <SDL2/SDL.h>


SDL_Surface* preprocess(SDL_Surface* image, double factor_size)
{

	
	Matrix* grayscaled = GrayscaleToMatrix(image);

	clock_t time1 = clock();
	Matrix* cannied = canny(grayscaled, 1);
	clock_t time2 = clock();
	printf("Time to canny : %fs\n", (double)(time2 - time1) / CLOCKS_PER_SEC);

	SDL_Surface* res = MatrixToSurface(cannied);

	return res;

}

