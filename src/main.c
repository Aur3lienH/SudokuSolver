#include <stdio.h>
#include "deepLearning/applications/Mnist.h"
#include "matrix/MatrixTests.h"
#include "UI/App.h"
#include "UI/DrawDigit.h"
#include "deepLearning/NetworkTest.h"
#include "tools/StringTools.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "sudokuSolver/Solver.h"
#include "deepLearning/Sudoku.h"
#include "Sudoku.h"
#include "imageProcessing/Preprocessing.h"
#include "imageProcessing/ImageProcessing.h"
#include "imageProcessing/DigitProcess.h"
#include "imageProcessing/SdlConverter.h"
#include "imageProcessing/DatasetCreation.h"
#include "deepLearning/applications/GridDetection.h"
#include "imageProcessing/SpeedTest.h"
#include "TestSudoku.h"

void PrintHelpFlag()
{
    printf("Usage: ./build/Solver [flags]\n");
    printf("Flags:\n");
    printf("\t-t, --train: Train the mnist model with Feed forward network\n");
    printf("\t--test: Test the matrix operations \n");
    printf("\t--test-AVX: Test the mnist model with AVX\n");
    printf("\t--ui: Launch the UI app\n");
    printf("\t--train-conv: Train the mnist model with convolutional layers\n");
    printf("\t--dataset [folder] [output] [size]: Create a dataset from a folder\n");
    printf("\t--train-grid [folder]: Train the grid recognition model\n");
    printf("\t--test-image [folder]: Test multiple images\n");
    printf("\t--draw-digit [image]: Draw a digit\n");
    printf("\t--help: Print this message\n");
}


void HandleFlags(int argc, char* argv[])
{
    int i = 1;
    while(i < argc)
    {
        if(CompareStrings(argv[i],"--help"))
        {
            PrintHelpFlag();
            break;
        }
        if(CompareStrings(argv[i],"-t") || CompareStrings(argv[i],"--train"))
        {
            Mnist_Train();
            break;
        }
        if(CompareStrings(argv[i],"-d") || CompareStrings(argv[i],"--draw-digit"))
        {
            DrawDigit(argc-2,argv+2,NULL);
            break;
        }

        if(CompareStrings(argv[i],"--test"))
        {
            M_Test();
            break;
        }
        if(CompareStrings(argv[i],"--test-AVX"))
        {
            M_AVX_Test();
            break;
        }

        if(CompareStrings(argv[i],"--load"))
        {
            Mnist_Load("./models/mnist.model");
            break;
        }

        if(CompareStrings(argv[i],"--ui"))
        {
            LaunchApp(argc-1,argv+1);
            break;
        }


        if(CompareStrings(argv[i],"--train-conv"))
        {
            Mnist_Train_ConvLayers();
        }

        // if(CompareStrings(argv[i],"--draw"))
        // {
        //     DrawDigit(argc,argv);
        // }


       

        if(CompareStrings(argv[i],"--dataset"))
        {
            if(argc >= 5)
            {
                CreateDataset(argv[i+1],argv[i+2],atoi(argv[i+3]));
            }
            else
            {
                printf("No folder provided\n");
            }
            return;
        }

        if(CompareStrings(argv[i],"--train-grid"))
        {
            if(argc >= 3)
            {
                TrainGridRecognition(argv[i+1]);
            }
            else
            {
                printf("No folder provided\n");
            }
        }

        //Test multiple images
        if(CompareStrings(argv[i], "--test-image"))
        {
            if(argc >= 3)
            {
                TestSudoku(argv[i+1]);
            }
            else
            {
                printf("No folder provided\n");
            }
        }

        i++;
    }
}

int main(int argc, char *argv[])
{
    HandleFlags(argc,argv);
    return 0;
}