#include "Sudoku.h"
#include "deepLearning/Network.h"
#include "deepLearning/Sudoku.h"
#include "imageProcessing/DigitProcess.h"
#include "sudokuSolver/Solver.h"
#include "deepLearning/applications/Mnist.h"
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
    SDL_Surface* image = IMG_Load(path);
    Matrix* grayscaled = M_Create_2D(image->h,image->w);
    time_t start = clock();
    GrayscaleToMatrix_C(image, grayscaled);
    printf("Grayscaling took %f seconds\n", (float)(clock() - start) / CLOCKS_PER_SEC);
    SaveMatrix(grayscaled,"images/export/preproc_0.jpg");
    Matrix* resized = resize(grayscaled, 500);
    Matrix* canny = Canny(resized, 1);
    Square square = GetSquareWithContour(canny);
    S_Sort(&square, canny);
    SDL_Surface* surface = MatrixToSurface(canny);
    P_DrawSDL(surface,&square.points[0],0xFF0000);
    P_DrawSDL(surface,&square.points[1],0xFF0000);
    P_DrawSDL(surface,&square.points[2],0xFF0000);
    P_DrawSDL(surface,&square.points[3],0xFF0000);
    IMG_SaveJPG(surface,"images/export/step_2.jpg",100);
    Matrix* perspectiveCorrected = TransformPerspective(resized, square,540);
    SaveMatrix(perspectiveCorrected,"images/export/step_3.jpg");

    Matrix** cells = SplitCells(perspectiveCorrected, 9); 

    



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

