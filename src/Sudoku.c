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
    // Load the best model to recognize digits
    Network* n = LoadBestRecognitionModel();

    // Load the image
    SDL_Surface* image = IMG_Load(path);

    //Convert the image to grayscale and downscale it
    Matrix* grayscaled = DownGrayscaleToMatrix(image, 500);

    Matrix* canny = Canny(grayscaled, 1);
    Square square = GetSquareWithContour(canny);
    S_Sort(&square, canny);

    //Write the square on the image and save it 
    SDL_Surface* surface = MatrixToSurface(canny);
    S_DrawSDL(surface,&square, 0xFF0000);
    IMG_SaveJPG(surface,"images/export/step_2.jpg",100);

    //Transform the image to a square
    Matrix* perspectiveCorrected = TransformPerspective(grayscaled, square,540);
    SaveMatrix(perspectiveCorrected,"images/export/step_3.jpg");



    //Split the image into 81 cells
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

