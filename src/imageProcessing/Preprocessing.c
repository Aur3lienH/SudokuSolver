#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Grayscale.h"
#include "Canny.h"
#include "Hough.h"
#include <time.h>
#include <SDL2/SDL.h>
#include "../geometry/Square.h"
#include "../geometry/Point.h"


SDL_Surface* preprocess(SDL_Surface* image, double factor_size)
{

	
	Matrix* grayscaled = GrayscaleToMatrix(image);

	clock_t time1 = clock();
	Matrix* cannied = canny(grayscaled, 1);
	clock_t time2 = clock();
	printf("Time to canny : %fs\n", (double)(time2 - time1) / CLOCKS_PER_SEC);
	
	Square square = Hough(cannied);
	printf("Square found\n");
	printf("Point 1: %d, %d\n", square.points[0].x, square.points[0].y);
	printf("Point 2: %d, %d\n", square.points[1].x, square.points[1].y);
	printf("Point 3: %d, %d\n", square.points[2].x, square.points[2].y);
	printf("Point 4: %d, %d\n", square.points[3].x, square.points[3].y);

	SDL_Surface* res = MatrixToSurface(cannied);
	return res;

}

