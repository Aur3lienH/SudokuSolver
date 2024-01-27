#include "imageProcessing/ImageProcessing.h"
#include "imageProcessing/Preprocessing.h"
#include "imageProcessing/Hough.h"
#include "imageProcessing/SquareDetection.h"
#include "geometry/Square.h"
#include <time.h>


Image* SudokuImgProcessing(Image* image)
{
    printf("Starting preprocessing\n");
    time_t start = clock();
    Matrix* res = PreprocessToCanny(image, 1);
    printf("Preprocessing took %f seconds\n", (float)(clock() - start) / CLOCKS_PER_SEC);
    Square square = GetSquareWithContour(res);
    //Square square2 = Hough(res);
    Image* surface = MatrixToImage(res);
    M_Free(res);
    P_DrawSDL(surface,&square.points[0],0xFF0000);
    P_DrawSDL(surface,&square.points[1],0xFF0000);
    P_DrawSDL(surface,&square.points[2],0xFF0000);
    P_DrawSDL(surface,&square.points[3],0xFF0000);

    
    return surface;
}

Matrix** SplitCells(Matrix* image, size_t cellCount)
{
    Matrix** cells = malloc(sizeof(Matrix*) * cellCount * cellCount);
    size_t cellSize = image->rows / cellCount;
    size_t cellSizeSquared = cellSize * cellSize;
    for (size_t h = 0; h < cellCount; h++)
    {
        for (size_t i = 0; i < cellCount; i++)
        {
            cells[i + h * cellCount] = M_Create_2D(cellSize,cellSize);
            for (size_t j = 0; j < cellSize; j++)
            {
                for (size_t k = 0; k < cellSize; k++)
                {
                    cells[i+h*cellCount]->data[j * cellSize + k] = image->data[(h * cellSize + j) * image->cols + i * cellSize + k];
                }
            }
        }
    }
    
    
    return cells;
}