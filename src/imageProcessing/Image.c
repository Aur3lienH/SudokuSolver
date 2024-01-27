#include "imageProcessing/Image.h"
#include "imageProcessing/stb_image.h"
#include "imageProcessing/Grayscale.h"
#include <err.h>
#include <stdlib.h>
#include <string.h>

Image* Image_Create(size_t width, size_t height, size_t channels)
{
#if __ANDROID__
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
#if __ANDROID__
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
#if __ANDROID__
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
        imageMobile->pixels[i];
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

Matrix* ImageToMatrix(Image* image)
{

}
