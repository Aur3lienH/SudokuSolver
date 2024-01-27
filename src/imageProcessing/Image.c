#include "imageProcessing/Image.h"
#include "imageProcessing/SdlConverter.h"
#include <err.h>
#include <stdlib.h>
#include <string.h>


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

Image* Image_Create(size_t width, size_t height, size_t channels)
{
#if MOBILE
    Image* img = malloc(sizeof(Image));
    img->width = width;
    img->height = height;
    img->pixels = malloc(sizeof(unsigned char) * width * height * channels);
    return img;
#else
    Image* img = malloc(sizeof(Image));
    return img;
#endif
}

Image* Image_Load(const char* path)
{
#if MOBILE
    Image* img = malloc(sizeof(Image));
    img->pixels = stbi_load(path, &img->width, &img->height, &img->channels, 4);
    printf("Loaded image with width %zu, height %zu and %zu channels\n", img->width, img->height, img->channels);
    return img;
#else
    return IMG_Load(path);
#endif
}


void Image_Free(Image* image)
{
#if MOBILE
    free(image->pixels);
    free(image);
#else
    free(image);
#endif
}

ImageMobile* MatrixToImageMobile(Matrix* matrix)
{
    ImageMobile* imageMobile = Image_Create(matrix->cols,matrix->rows,4);
	
	for (size_t i = 0; i < matrix->cols * matrix->rows; i++)
    {
        unsigned char value = matrix->data[i] * 255;
        imageMobile->pixels[i*4] = value;
        imageMobile->pixels[i*4+1] = value;
        imageMobile->pixels[i*4+2] = value;
        imageMobile->pixels[i*4+3] = 255;
    }
}

Image* MatrixToImage(Matrix* matrix)
{
#if MOBILE
    return MatrixToImageMobile(matrix);
#else
    return MatrixToSurface(matrix);
#endif
}

Matrix* ImageMobileToMatrix(ImageMobile* imageMobile)
{
    Matrix* res = M_Create_2D(imageMobile->height,imageMobile->width);
    size_t length = M_GetSize2D(res);
    for (size_t i = 0; i < length; i++)
    {
        size_t index = i * 4;
        float value = 0.0f;
        value += (imageMobile->pixels[i] / 255.0f) * 0.2989;
        value += (imageMobile->pixels[i+1] / 255.0f) * 0.5870;
        value += (imageMobile->pixels[i+2] / 255.0f) * 0.1140;
        res->data[i] = value;
    }
    return res;
    
}

Matrix* ImageToMatrix(Image* image)
{
#if MOBILE
    return ImageMobileToMatrix(image);
#else
    return GrayscaleToMatrix(image);
#endif
}


void M_SaveImage(const Matrix* matrix, const char* path)
{
    Image* image = MatrixToImage(matrix);
    Image_Save(image,path);
}


void Image_Save(Image* image, const char* path)
{
#if MOBILE
    stbi_write_jpg(path,image->width,image->height,image->channels,image->pixels,100);
#else
	IMG_SaveJPG(image, path, 100);
#endif
}
