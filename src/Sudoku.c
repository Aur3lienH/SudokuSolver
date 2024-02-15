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
#include "imageProcessing/Image.h"
#include "imageProcessing/AddDigit.h"
#include "imageProcessing/Canny.h"
#include "geometry/Square.h"    
#include "geometry/Point.h"
#include <time.h>
#include <stdio.h>


const size_t PERSPECTIVE_WIDTH = 540;

int** GetResolvedSudoku(char* path, int* success)
{
    //Load the model
    Network* n = LoadBestRecognitionModel();
    //Load the image
    Image* image = Image_Load(path);
    //Convert the image to grayscale and downscale it
    Matrix* matrix = ImageTo3DMatrix(image);

    M_SaveImage3D(matrix, "images/export/initial.jpg");

    Matrix* resized = resize3D(matrix,540);

    M_SaveImage3D(resized, "images/export/resized1.jpg");

    Matrix* grayscaled = M_Grayscale(resized);

    Matrix* canny = Canny(grayscaled, 1);

    SquareDetectionResult sdr = GetSquareWithContour(canny);
    //S_Draw(resized, &sdr.square, Color_Create(0,0,255), 0);

    //M_AdjustBorder(resized, sdr);


    
    //double* justToRotate = CalculateH(sdr.square, WidthToSquare(PERSPECTIVE_WIDTH));

    double* hForward = CalculateH(sdr.square, WidthToSquare(PERSPECTIVE_WIDTH));

    M_SaveImage3D(resized, "images/export/preproc_0.jpg");

    Matrix* perspectiveCorrected = TransformPerspectiveColor_I(resized, PERSPECTIVE_WIDTH, hForward);

    M_SaveImage3D(perspectiveCorrected, "images/export/step_3.jpg");

    Matrix* copy = M_CopyI(resized);
    S_Draw(copy, &sdr.square, Color_Create(0,0,255), 0);
    M_SaveImage3D(copy, "images/export/step_2.jpg");

    Matrix* perspectiveCorrectedGrayscale = M_Grayscale(perspectiveCorrected);

    M_SaveImage(perspectiveCorrectedGrayscale, "images/export/grayscale.jpg");

    Matrix** cells = SplitCells(perspectiveCorrectedGrayscale, 9);

    int** sudoku = GetSudokuNumbers(n, cells);
    int grid[9][9];
    for (size_t i = 0; i < 9; i++)
    {
        for (size_t j = 0; j <9; j++)
        {
            grid[i][j] = sudoku[i][j];
        }
        
    }

    int** missingDigits = malloc(sizeof(int*) * 9);

    for (size_t i = 0; i < 9; i++)
    {
        missingDigits[i] = malloc(sizeof(int) * 9);
        for (size_t j = 0; j < 9; j++)
        {
            missingDigits[i][j] = 0;
        }
        
    }

    print_grid(grid);
    int error = getErrorSudoku(grid, missingDigits);
    if(error == 1)
    {
        OverlignMatrix(perspectiveCorrected, missingDigits);
    }
    else
    {
        int resolved = solverMissingDigits(grid, missingDigits);
        AddMissingDigits(perspectiveCorrected,missingDigits,PERSPECTIVE_WIDTH / 9,Color_Create(0,255,0));   
    }

    M_SaveImage3D(perspectiveCorrected, "images/export/prevFinal.jpg");

    double* hReverse = CalculateH(WidthToSquare(PERSPECTIVE_WIDTH), sdr.square);

    TransformPerspectiveColor(perspectiveCorrected, resized, hReverse);

    M_SaveImage3D(resized, "images/export/step_4.jpg");

    *success = 1;
    for (size_t i = 0; i < 9; i++)
    {
        for (size_t j = 0; j < 9; j++)
        {
            if(grid[i][j] == 0)
            {
                *success = 0;
            }
        }
    }

    return sudoku;
    
}




int** ImageToSudoku(char* path)
{
    int success = 0;
    return GetResolvedSudoku(path, &success);

    /*
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
    Matrix* perspectiveCorrected = TransformPerspective(resized, square,PERSPECTIVE_WIDTH);
    M_SaveImage(perspectiveCorrected,"images/export/step_3.jpg");

    //Split the image into 81 cells
    Matrix** cells = SplitCells(perspectiveCorrected, 9); 


    int** sudoku = GetSudokuNumbers(n, cells);
    printf("Sudoku numbers detected\n");

    int** missingDigits = malloc(sizeof(int*) * 9);
    for (size_t i = 0; i < 9; i++)
    {
        missingDigits[i] = malloc(sizeof(int) * 9);
        for (size_t j = 0; j < 9; j++)
        {
            missingDigits[i][j] = 0;
        }
    } 
    int grid[9][9];
    for (size_t i = 0; i < 9; i++)
    {
        for (size_t j = 0; j < 9; j++)
        {
                
            grid[i][j] = sudoku[i][j];
        }
        
    }
    




    int resolved = solverMissingDigits(grid, missingDigits);
    if(resolved == 0)
    {
        printf("Sudoku not resolved\n");
    }
    AddMissingDigits(perspectiveCorrected,missingDigits,PERSPECTIVE_WIDTH / 9, Color_Create(0,255,0));
    M_SaveImage(perspectiveCorrected,"images/export/step_4.jpg");
    
    for (size_t i = 0; i < 9; i++)
    {
        for (size_t j = 0; j < 9; j++)
        {
            printf("%i ", sudoku[i][j]);
        }
        printf("\n");
    }
    
    return sudoku;
    */
}

