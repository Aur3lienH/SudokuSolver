#include "Sudoku.h"
#include  "imageProcessing/DigitProcess.h"
#include "deepLearning/Network.h"
#include "matrix/Matrix.h"
#include "deepLearning/applications/Mnist.h"
#include "imageProcessing/Binarisation.h"
#include "imageProcessing/Grayscale.h"
#include <stdlib.h>

const int modelDetectingBlank = 0;



int GetNumber(Network* n,Matrix* matrix, size_t id)
{
    int isBlank;
    char path[1024];
    snprintf(path, sizeof(path), "images/export/step_%li.jpg", id);
    M_SaveImage(matrix, path);
    Matrix* preprocessed = M_OptimalBinarisation(matrix);
    snprintf(path, sizeof(path), "images/export/step_%li.jpg", id+1);
    M_SaveImage(preprocessed, path);
    Matrix* final = MatrixToDigit(preprocessed,&isBlank);
    snprintf(path, sizeof(path), "images/export/step_%li.jpg", id+2);
    M_SaveImage(final, path);

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
            res[i][j] = GetNumber(n, img[i * 9 + j], (i * 9 + j + 1) * 3 + 1);
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
