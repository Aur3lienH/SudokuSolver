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
#include "SquareDetection.h"



int filterImage(const struct dirent* name)
{
    if(strstr(name->d_name,".jpg") != NULL || strstr(name->d_name,".jpeg") != NULL || strstr(name->d_name,".png") != NULL)
    {
        return 1;
    }
    return 0;
}

Matrix* SquareToMatrix(Square square)
{
    Matrix* res = M_Create_2D(8,1);
    res->data[0] = square.points[0].x;
    res->data[1] = square.points[0].y;
    res->data[2] = square.points[1].x;
    res->data[3] = square.points[1].y;
    res->data[4] = square.points[2].x;
    res->data[5] = square.points[2].y;
    res->data[6] = square.points[3].x;
    res->data[7] = square.points[3].y;
    return res;
}


void ImageProcess(const char* path,const char* outputPath,FILE* datasetFile, const char* filename, size_t width)
{
    SDL_Surface* image = IMG_Load(path);
    if(image == NULL)
    {
        printf("Error while loading the image\n");
        exit(-1);
    }
    Matrix* grayscaled = GrayscaleToMatrix(image);
	Matrix* resized = resize(grayscaled,width);
    Matrix* cannied = Canny(resized, 2);
    Square square = GetSquareWithContour(cannied);
    Matrix* saveMatrix = M_Complete(resized,width,width);

    SDL_Surface* surface = MatrixToSurface(saveMatrix);
    P_DrawSDL(surface,&square.points[0],0xFF0000);
    P_DrawSDL(surface,&square.points[1],0xFF0000);
    P_DrawSDL(surface,&square.points[2],0xFF0000);
    P_DrawSDL(surface,&square.points[3],0xFF0000);
    char* savePath = malloc(sizeof(char) * 100);
    snprintf(savePath, sizeof(savePath),"%s/%s",outputPath,filename);
    IMG_SaveJPG(surface,savePath,100);

    //Write the matrix after it has been resized
    M_Save(saveMatrix,datasetFile);
    Matrix* squareMatrix = SquareToMatrix(square);
    M_Save(squareMatrix,datasetFile);
    M_Free(squareMatrix);
    
}


void CreateDataset(const char* inputFolder, const char* outputFolder, size_t width)
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
    char* outputFolderCopy = malloc(sizeof(char) * 100);
    strcpy(outputFolderCopy,outputFolder);
    strcat((char *)outputFolder,"/dataset.dat");
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
    res = fwrite(&width,sizeof(int),1,datasetFile);
    if(res != 1)
    {
        printf("Error while writing the width\n");
        exit(-1);
    }
    for (int i = 0; i < n; i++)
    {
        char* path = malloc(sizeof(char) * 100);
        snprintf(path, sizeof(path),"%s/%s",inputFolder,namelist[i]->d_name);
        ImageProcess(path,outputFolderCopy,datasetFile,namelist[i]->d_name,width);  
        free(path);
    }
}