#include <stdio.h>
#include "DeepLearning/Mnist.h"
#include "DeepLearning/MatrixTests.h"
#include "UI/App.h"
#include "UI/DrawDigit.h"
#include "DeepLearning/NetworkTest.h"
#include "Tools.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "SudokuSolver/Solver.h"
#include "DeepLearning/Sudoku.h"
#include "Sudoku.h"
#include "ImageProcessing/Preprocessing.h"


void HandleFlags(int argc, char* argv[])
{
    int i = 1;
    while(i < argc)
    {
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

        if(CompareStrings(argv[i],"-r"))
        {
            Rotation_Train();
            break;
        }

        if(CompareStrings(argv[i],"--test-net"))
        {
            NetworkTest();
            break;
        }

        if(CompareStrings(argv[i],"--solver"))
        {
            solver_main(argc,argv);
            break;
        }

        if(CompareStrings(argv[i],"--toSudoku"))
        {
            ToSudoku();
        }

        if(CompareStrings(argv[i],"--train-conv"))
        {
            Mnist_Train_ConvLayers();
        }

        // if(CompareStrings(argv[i],"--draw"))
        // {
        //     DrawDigit(argc,argv);
        // }

        if(CompareStrings(argv[i],"--num"))
        {
            if(argc >= 3)
            {
                ImageToSudoku(argv[i+1]);
            }
            else
            {
                printf("No file provided\n");
            }
        }

        if(CompareStrings(argv[i],"--preprocess"))
        {
            if(argc >= 3)
            {
                SDL_Surface* img = IMG_Load(argv[i+1]);

                img = preprocess(img,1.2);
                IMG_SaveJPG(img,"images/preprocessed/IMG1.jpg",100);
                SDL_FreeSurface(img);
            }
            else
            {
                printf("No file provided\n");
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