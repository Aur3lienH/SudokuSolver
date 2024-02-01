#include "imageProcessing/Image.h"
#include "imageProcessing/SdlConverter.h"
#include <err.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "imageProcessing/stb_image.h"
#include "imageProcessing/stb_image_write.h"

#ifndef MOBILE
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif

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
    //Load the image with stb_image and put it in the orientation of the image
    ImageMobile* img = malloc(sizeof(ImageMobile));
    img->pixels = stbi_load(path, &img->width, &img->height, &img->channels, 4);
    img->channels = (int)3;

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
    return imageMobile;
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
        value += (imageMobile->pixels[index] / 255.0f) * 0.2989;
        value += (imageMobile->pixels[index+1] / 255.0f) * 0.5870;
        value += (imageMobile->pixels[index+2] / 255.0f) * 0.1140;
        res->data[i] = value;
    }
    return res;
}


Matrix* ImageMobileTo3DMatrix(Image* image)
{
    Matrix* res = M_Create_3D(image->height,image->width,3);
    size_t length = image->height * image->width;
    for (size_t i = 0; i < length; i++)
    {

        size_t index = i * 4;
        res->data[i*3] = image->pixels[index] / 255.0f;
        res->data[i*3+1] = image->pixels[index+1] / 255.0f;
        res->data[i*3+2] = image->pixels[index+2] / 255.0f;
    }
    return res;
}


Matrix* ImageTo3DMatrix(Image* image)
{
#if MOBILE
    return ImageMobileTo3DMatrix(image);
#else
    return SurfaceTo3DImage(image);
#endif
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

void M_SaveImage3D(const Matrix* matrix, const char* path)
{
    Image* image = Matrix3DToImage(matrix);
    Image_Save(image,path);
}


Image* Matrix3DToImage(Matrix* matrix)
{
#if MOBILE
    Image* image = Image_Create(matrix->cols,matrix->rows,4);
    size_t length = M_GetSize2D(matrix);
    for (size_t i = 0; i < length; i++)
    {
        size_t index = i * 3;
        image->pixels[i*4] = (unsigned char)(matrix->data[index] * 255.0f);
        image->pixels[i*4+1] = (unsigned char)(matrix->data[index+1] * 255.0f);
        image->pixels[i*4+2] = (unsigned char)(matrix->data[index+2] * 255.0f);
        image->pixels[i*4+3] = 255;
    }
    return image;
#else
    return Matrix3DToSurface(matrix);
#endif
}

void Image_Save(Image* image, const char* path)
{
#if MOBILE
    stbi_write_jpg(path,image->width,image->height,4,image->pixels,100);
#else
	IMG_SaveJPG(image, path, 100);
#endif
}


Color Color_Create(unsigned char r, unsigned char g, unsigned char b)
{
    Color color = {r,g,b};
    return color;
}