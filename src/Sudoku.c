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

char* GetResolvedSudoku(char* path)
{
    //Load the model
    Network* n = LoadBestRecognitionModel();
    //Load the image
    Image* image = Image_Load(path);
    //Convert the image to grayscale and downscale it
    Matrix* matrix = ImageTo3DMatrix(image);

    Matrix* resized = resize3D(matrix,500);

    Matrix* grayscaled = M_Grayscale(resized);

    Matrix* canny = Canny(grayscaled, 1);

    Square square = GetSquareWithContour(canny);
    

    double* h = CalculateH(square, 540);
    double* inversedH = InverseH(h);

    Matrix* perspectiveCorrected = TransformPerspectiveColor_I(resized, PERSPECTIVE_WIDTH, inversedH);

    M_SaveImage3D(perspectiveCorrected, "images/export/corrected.jpg");

    M_Dim(perspectiveCorrected);

    Matrix* perspectiveCorrectedGrayscale = M_Grayscale(perspectiveCorrected);

    M_SaveImage(perspectiveCorrectedGrayscale, "images/export/grayscale.jpg");

    Matrix** cells = SplitCells(perspectiveCorrectedGrayscale, 9);

    int** sudoku = GetSudokuNumbers(n, cells);
    int grid[9][9];
    for (size_t i = 0; i < 9; i++)
    {
        for (size_t j = 0; j <9; j++)
        {
            grid[i][j] = 0;
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

    int resolved = solverMissingDigits(grid, missingDigits);

    if(resolved == 0)
    {
        printf("Sudoku not resolved\n");
    }

    printf("sovled\n");
    AddMissingDigits(perspectiveCorrected,missingDigits,PERSPECTIVE_WIDTH / 9,Color_Create(0,255,0));

    TransformPerspectiveColor(perspectiveCorrected, resized, h);

    M_SaveImage(resized, "images/export/final.jpg");
    
}

int** ImageToSudoku(char* path)
{

    GetResolvedSudoku(path);

    return NULL;
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

