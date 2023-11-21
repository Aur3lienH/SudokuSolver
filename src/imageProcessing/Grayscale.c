#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../deepLearning/Matrix.h"


Matrix* GrayscaleToMatrix(SDL_Surface* image)
{
	SDL_Surface* convertedSurface = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGBA32, 0);
	Matrix* res = M_Create_2D(image->h,image->w);
	if (SDL_MUSTLOCK(image)) {
		SDL_LockSurface(image);
	}
	Uint32 *pixels = (Uint32 *)convertedSurface->pixels;
	for (int i = 0; i < image->w * image->h; i++) {
		
		Uint8 r, g, b, a;
		SDL_GetRGBA(pixels[i], convertedSurface->format, &r, &g, &b, &a);
		Uint8 average = (int)(0.2989 * r + 0.5870 * g + 0.1140 * b);
		res->data[i] = (float)average / 255.0f;
	}
	  if (SDL_MUSTLOCK(image)) {
		SDL_UnlockSurface(image);
	}
	return res;
}


SDL_Surface* MatrixToSurface(Matrix* matrix)
{
	SDL_Surface* res = SDL_CreateRGBSurface(0, matrix->cols, matrix->rows, 32, 0, 0, 0, 0);
	if (SDL_MUSTLOCK(res)) {
		SDL_LockSurface(res);
	}
	Uint32 *pixels = (Uint32 *)res->pixels;
	for (int i = 0; i < matrix->rows * matrix->cols; i++) {
		Uint8 value = (Uint8)(matrix->data[i] * 255.0f);
		pixels[i] = SDL_MapRGBA(res->format, value, value, value, 255);
	}
	  if (SDL_MUSTLOCK(res)) {
		SDL_UnlockSurface(res);
	}
	return res;
}

