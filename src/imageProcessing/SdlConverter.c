
#include "imageProcessing/SdlConverter.h"

#ifndef __ANDROID__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "matrix/Matrix.h"



void DownGrayscaleToMatrix_C(SDL_Surface* image, Matrix* res)
{
	printf("DownGrayscaleToMatrix_C\n");
	size_t maximum = image->w > image->h ? image->w : image->h;
	float ratio = (float)res->cols / (float)maximum;
	SDL_Surface* convertedSurface = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGBA32, 0);
	if (SDL_MUSTLOCK(image)) {
		SDL_LockSurface(image);
	}
	Uint32 *pixels = (Uint32 *)convertedSurface->pixels;
	for (size_t i = 0; i < res->rows; i++)
	{
		for(size_t j = 0; j < res->cols; j++)
		{
			Uint8 r, g, b, a;

			float indexCol = floor(j / ratio);
			float indexRow = floor(i / ratio);
			SDL_GetRGBA(pixels[(int)indexRow * image->w + (int)indexCol], convertedSurface->format, &r, &g, &b, &a);
			Uint8 average = (int)(0.2989 * r + 0.5870 * g + 0.1140 * b);
			res->data[i * res->cols + j] = (float)average / 255.0f;
		}
	}
	if (SDL_MUSTLOCK(image)) {
		SDL_UnlockSurface(image);
	}
}

Matrix* DownGrayscaleToMatrix(SDL_Surface* image, size_t width)
{
	Matrix* res = M_Create_2D(width,width);
	DownGrayscaleToMatrix_C(image, res);
	return res;
}


void GrayscaleToMatrix_C(SDL_Surface* image, Matrix* res)
{
	SDL_Surface* convertedSurface = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGBA32, 0);
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
}



Matrix* GrayscaleToMatrix(SDL_Surface* image)
{
	Matrix* res = M_Create_2D(image->h,image->w);
	GrayscaleToMatrix_C(image, res);
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

Matrix* SurfaceTo3DImage(SDL_Surface surface)
{
	Matrix* res = M_Create_3D(surface.h, surface.w, 3);
	if (SDL_MUSTLOCK(&surface)) {
		SDL_LockSurface(&surface);
	}
	Uint32 *pixels = (Uint32 *)surface.pixels;
	for (int i = 0; i < surface.w * surface.h; i++) {
		Uint8 r, g, b, a;
		SDL_GetRGBA(pixels[i], surface.format, &r, &g, &b, &a);
		res->data[i * 3] = (float)r / 255.0f;
		res->data[i * 3 + 1] = (float)g / 255.0f;
		res->data[i * 3 + 2] = (float)b / 255.0f;
	}
	  if (SDL_MUSTLOCK(&surface)) {
		SDL_UnlockSurface(&surface);
	}
	return res;
}

#endif

