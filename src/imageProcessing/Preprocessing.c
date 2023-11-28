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

const size_t DownsizeWidth = 500;

Matrix* resize(const Matrix* input)
{
	size_t maximum = input->cols > input->rows ? input->cols : input->rows;
	printf("Maximum : %ld\n", maximum);
	float ratio = (float)DownsizeWidth / (float)maximum;
	printf("Ratio : %f\n", ratio);
	return DownScale(input,ratio);
}

Matrix* preprocess(SDL_Surface* image, double factor_size)
{
	
	
	Matrix* grayscaled = GrayscaleToMatrix(image);
	Matrix* resized = resize(grayscaled);
	clock_t time1 = clock();
	Matrix* cannied = canny(resized, 2);
	clock_t time2 = clock();
	printf("Time to canny : %fs\n", (double)(time2 - time1) / CLOCKS_PER_SEC);
	
	//Square square = Hough(cannied);
	printf("Square found\n");
	//printf("Point 1: %d, %d\n", square.points[0].x, square.points[0].y);
	//printf("Point 2: %d, %d\n", square.points[1].x, square.points[1].y);
	//printf("Point 3: %d, %d\n", square.points[2].x, square.points[2].y);
	//printf("Point 4: %d, %d\n", square.points[3].x, square.points[3].y);

	return cannied;
}

