#include "Sudoku.h"
#include "deepLearning/Network.h"
#include "deepLearning/Sudoku.h"
#include "imageProcessing/DigitProcess.h"
#include "sudokuSolver/Solver.h"
#include "deepLearning/applications/Mnist.h"
#include "imageProcessing/Preprocessing.h"
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
    Image* image = Image_Load(path);
    //Convert the image to grayscale and downscale it
    Matrix* grayscaled = ImageToMatrix(image);
    M_Dim(grayscaled);
    Matrix* resized = resize(grayscaled,500);

    Matrix* canny = Canny(resized, 1);
    Square square = GetSquareWithContour(canny);
    S_Sort(&square, canny);

    //Write the square on the image and save it 
    Image* imageSave = MatrixToImage(canny);
    S_DrawSDL(imageSave,&square, 0xFF0000);
    Image_Save(imageSave,"images/export/step_2.jpg");
    printf("Square detected\n");
    //Transform the image to a square
    Matrix* perspectiveCorrected = TransformPerspective(resized, square,540);
    M_SaveImage(perspectiveCorrected,"images/export/step_3.jpg");


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

