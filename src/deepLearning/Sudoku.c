#include "Sudoku.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../Downscale.h"
#include "Network.h"
#include "Matrix.h"
#include "Mnist.h"
#include "../Downscale.h"
#include <stdlib.h>

const int modelDetectingBlank = 0;



int GetNumber(Network* n,SDL_Surface *img)
{
    int isBlank;
    Matrix* matrix = SurfaceToDigit(img,&isBlank);
    matrix->rows = 784;
    matrix->cols = 1;
    if (isBlank && !modelDetectingBlank)
    {
        M_Free(matrix);
        return 0;
    }
    else
    {
        M_Dim(matrix);
        int res = S_MatrixToLabel(N_Process(n, matrix));
        M_Free(matrix);
        return res;
    }

}

int** GetSudokuNumbers(Network* n, SDL_Surface **img)
{
    int** res = (int**)malloc(sizeof(int*) * 9);
    for (size_t i = 0; i < 9; i++)
    {
        res[i] = (int*)malloc(sizeof(int) * 9);
        for (size_t j = 0; j < 9; j++)
        {
            res[i][j] = GetNumber(n, img[i * 9 + j]);
        }
    }
    return res;
}

int** GetSudokuNumbers_File(Network* n)
{
    char* path;
    SDL_Surface** img = (SDL_Surface**)malloc(sizeof(SDL_Surface*) * 81);
    for (size_t i = 0; i < 81; i++)
    {
        path = (char*)malloc(sizeof(char) * 1024);
        sprintf(path, "images/cells/cell_%li.jpg", i);
        img[i] = IMG_Load(path);
        free(path);
    }
    int** res = GetSudokuNumbers(n, img);
    for (size_t i = 0; i < 81; i++)
    {
        SDL_FreeSurface(img[i]);
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
