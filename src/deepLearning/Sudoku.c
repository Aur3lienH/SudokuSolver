#include "Sudoku.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../Downscale.h"
#include "Network.h"
#include "Matrix.h"
#include "Mnist.h"
#include "../Downscale.h"
#include "../imageProcessing/Binarisation.h"
#include "../imageProcessing/Grayscale.h"
#include <stdlib.h>

const int modelDetectingBlank = 0;



int GetNumber(Network* n,Matrix* matrix)
{
    int isBlank;
    Matrix* preprocessed = M_OptimalBinarisation(matrix);
    Matrix* final = MatrixToDigit(preprocessed,&isBlank);
    char path [1024];
    snprintf(path, sizeof(path), "images/cell_%i.jpg",rand());
    IMG_SaveJPG(MatrixToSurface(preprocessed),path,100);

    final->rows = 784;
    final->cols = 1;
    if (isBlank && !modelDetectingBlank)
    {
        M_Free(matrix);
        return 0;
    }
    else
    {
        int res = S_MatrixToLabel(N_Process(n, final));
        M_Free(matrix);
        M_Free(final);
        return res;
    }

}

int** GetSudokuNumbers(Network* n, Matrix **img)
{
    int** res = (int**)malloc(sizeof(int*) * 9);
    for (size_t i = 0; i < 9; i++)
    {
        res[i] = (int*)malloc(sizeof(int) * 9);
        for (size_t j = 0; j < 9; j++)
        {
            char* path = (char*)malloc(sizeof(char) * 1024);
            snprintf(path, 1024, "./images/cell_%li.jpg", i * 9 + j);
            IMG_SaveJPG(MatrixToSurface(img[i * 9 + j]), path, 100);
            SDL_Surface* surface = IMG_Load(path);
            res[i][j] = GetNumber(n, img[i * 9 + j]);
        }
    }
    return res;
}

int** GetSudokuNumbers_File(Network* n)
{
    char* path;
    Matrix** img = (Matrix**)malloc(sizeof(Matrix*) * 81);
    for (size_t i = 0; i < 81; i++)
    {
        path = (char*)malloc(sizeof(char) * 1024);
        snprintf(path, sizeof(path), "images/cells/cell_%li.jpg", i);
        SDL_Surface* surface = IMG_Load(path);
        img[i] = GrayscaleToMatrix(surface);
        free(path);
    }
    int** res = GetSudokuNumbers(n, img);
    for (size_t i = 0; i < 81; i++)
    {
        M_Free(img[i]);
    }
    free(img);
    return res;    
}

void ToSudoku()
{
    Network* n = LoadBestRecognitionModel();
    int** sudoku = GetSudokuNumbers_File(n);
    for (size_t i = 0; i < 9; i++)
    {
        printf("|");
        for (size_t j = 0; j < 9; j++)
        {
            printf("%i|",sudoku[i][j]);
        }
        printf("\n");
    }
    N_Free(n);
}
