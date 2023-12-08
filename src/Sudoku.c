#include "Sudoku.h"
#include "deepLearning/Network.h"
#include "deepLearning/Sudoku.h"
#include "Downscale.h"
#include "sudokuSolver/Solver.h"
#include "deepLearning/Mnist.h"
#include "imageProcessing/Preprocessing.h"
#include "imageProcessing/Grayscale.h"
#include "imageProcessing/SquareDetection.h"
#include "imageProcessing/TransformPerspective.h"
#include "imageProcessing/ImageProcessing.h"
#include "imageProcessing/Canny.h"
#include "geometry/Square.h"    
#include "geometry/Point.h"
#include <time.h>
#include <stdio.h>

void SaveMatrix(Matrix* matrix, char* path)
{
    SDL_Surface* surface = MatrixToSurface(matrix);
    IMG_SaveJPG(surface,path,100);
}

int** ImageToSudoku(char* path)
{
    Network* n = LoadBestRecognitionModel();
    printf("Model loaded\n");
    SDL_Surface* image = IMG_Load(path);
    Matrix* grayscaled = GrayscaleToMatrix(image);
    SaveMatrix(grayscaled,"images/export/preproc_0.jpg");
    Matrix* resized = resize(grayscaled, 500);
    Matrix* canny = Canny(resized, 1);
    SaveMatrix(canny,"images/export/preproc_1.jpg");
    printf("Canny applied\n");
    Square square = GetSquareWithContour(canny);
    S_Sort(&square, canny);
    SDL_Surface* surface = MatrixToSurface(canny);
    P_DrawSDL(surface,&square.points[0],0xFF0000);
    P_DrawSDL(surface,&square.points[1],0xFF0000);
    P_DrawSDL(surface,&square.points[2],0xFF0000);
    P_DrawSDL(surface,&square.points[3],0xFF0000);
    IMG_SaveJPG(surface,"images/export/step_1.jpg",100);
    Matrix* perspectiveCorrected = TransformPerspective(resized, square,540);
    SaveMatrix(perspectiveCorrected,"images/export/step_2.jpg");

    Matrix** cells = SplitCells(perspectiveCorrected, 9); 
    for (size_t i = 0; i < 81; i++)
    {
        char path [1024];
        snprintf(path, sizeof(path), "images/export/step_%li.jpg", i+3);
        IMG_SaveJPG(MatrixToSurface(cells[i]),path,100);
    }
    


    printf("Cells splitted\n");

    int** sudoku = GetSudokuNumbers(n, cells);
    printf("Sudoku numbers detected\n");

 

    for (size_t i = 0; i < 9; i++)
    {
        for (size_t j = 0; j < 9; j++)
        {
            printf("%i ", sudoku[i][j]);
        }
        printf("\n");
    }
    

    
    return sudoku;
}

