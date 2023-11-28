#include "DatasetCreation.h"
#include "Hough.h"
#include <stdio.h>
#include <stdlib.h> 
#include <dirent.h>
#include <string.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include "../Downscale.h"
#include "../geometry/Square.h"
#include "../geometry/Point.h"
#include "../deepLearning/ImageProcessing.h"
#include "Grayscale.h"
#include "Preprocessing.h"
#include "Canny.h"



int filterImage(const struct dirent* name)
{
    if(strstr(name->d_name,".jpg") != NULL || strstr(name->d_name,".jpeg") != NULL || strstr(name->d_name,".png") != NULL)
    {
        return 1;
    }
    return 0;
}

void ImageProcess(const char* path,const char* outputPath,FILE* datasetFile)
{
    SDL_Surface* image = IMG_Load(path);
    if(image == NULL)
    {
        printf("Error while loading the image\n");
        exit(-1);
    }
    Matrix* grayscaled = GrayscaleToMatrix(image);
	Matrix* resized = resize(grayscaled);
    Matrix* cannied = canny(resized, 2);
    Square square = Hough(cannied);
    //Write the points in the file
    for (size_t i = 0; i < 4; i++)
    {
        fwrite(&square.points[i].x,sizeof(int),1,datasetFile);
    }
    Matrix* saveMatrix = M_Complete(resized,500,500);

    SDL_Surface* surface = MatrixToSurface(saveMatrix);
    P_DrawSDL(surface,&square.points[0],0xFF0000);
    P_DrawSDL(surface,&square.points[1],0xFF0000);
    P_DrawSDL(surface,&square.points[2],0xFF0000);
    P_DrawSDL(surface,&square.points[3],0xFF0000);
    char* savePath = malloc(sizeof(char) * 100);
    sprintf(savePath,"%s.jpg",outputPath);
    printf("Saving %s\n",savePath);
    IMG_SaveJPG(surface,savePath,100);

    //Write the matrix after it has been resized
    M_Save(saveMatrix,datasetFile);
    
}


void CreateDataset(const char* inputFolder, const char* outputFolder)
{
    struct dirent **namelist;
    int n = scandir(inputFolder, &namelist, filterImage, alphasort);
    if (n == -1) {
        perror("scandir");
        exit(EXIT_FAILURE);
    }
    if(n == 0)
    {
        printf("No model found !\n");
        exit(-1);
    }
    strcat(outputFolder,"/dataset.dat");
    FILE* datasetFile = fopen(outputFolder,"wb");
    if(datasetFile == NULL)
    {
        printf("Error while opening the file\n");
        exit(-1);
    }
    int res = fwrite(&n,sizeof(int),1,datasetFile);
    if(res != 1)
    {
        printf("Error while writing the number of images\n");
        exit(-1);
    }
    for (int i = 0; i < n; i++)
    {
        char* path = malloc(sizeof(char) * 100);
        sprintf(path,"%s/%s",inputFolder,namelist[i]->d_name);
        printf("Processing %s\n",path);
        ImageProcess(path,datasetFile);
        free(path);
    }
}