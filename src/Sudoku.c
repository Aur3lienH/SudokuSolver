#include "Sudoku.h"
#include "DeepLearning/Network.h"
#include "DeepLearning/Sudoku.h"
#include "Downscale.h"
#include "SudokuSolver/Solver.h"
#include "DeepLearning/Mnist.h"
#include "ImageProcessing/Preprocessing.h"
#include <time.h>


int** ImageToSudoku(char* path)
{
    const clock_t time1 = clock();
    Network* n = LoadBestRecognitionModel();
    const clock_t time2 = clock();
    SDL_Surface* img = IMG_Load(path);
    const clock_t time3 = clock();
    img = preprocess(img, 5);
    const clock_t time4 = clock();
    SDL_Surface** cells = NULL;// Must be changed to get all the squares
    const clock_t time5 = clock();
    int** res = GetSudokuNumbers(n, cells);
    const clock_t time6 = clock();
    for (size_t i = 0; i < 81; i++)
    {
        SDL_FreeSurface(cells[i]);
    }
    SDL_FreeSurface(img);
    free(cells);
    for (size_t i = 0; i < 9; i++)
    {
        printf("|");
        for (size_t j = 0; j < 9; j++)
        {
            printf("%i|",res[i][j]);
        }
        printf("\n");
    }

    printf("Time to load model : %fs\n", (double)(time2 - time1) / CLOCKS_PER_SEC);
    printf("Time to load image : %fs\n", (double)(time3 - time2) / CLOCKS_PER_SEC);
    printf("Time to preprocess : %fs\n", (double)(time4 - time3) / CLOCKS_PER_SEC);
    printf("Time to hough : %fs\n", (double)(time5 - time4) / CLOCKS_PER_SEC);
    printf("Time to recognize : %fs\n", (double)(time6 - time5) / CLOCKS_PER_SEC);
    printf("Total time : %fs\n", (double)(time6 - time1) / CLOCKS_PER_SEC);
    

    N_Free(n);
    return res;
}

