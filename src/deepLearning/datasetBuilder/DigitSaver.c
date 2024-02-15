#include "deepLearning/datasetBuilder/DigitSaver.h"
#include "matrix/Matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


const char* UNSURE_DIGIT_PATH = "datasets/unsure.data";


char* timeToFilename()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char* time = malloc(100* sizeof(char));
    sprintf(time, "datasets/digits/%d-%d-%d_%d-%d-%d.digits", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    return time;
}


void SaveDigits(Matrix** matrix, unsigned char* supposedDigit, size_t matrixCount)
{

    FILE* file = fopen(UNSURE_DIGIT_PATH, "awb");
    if(file == NULL)
    {
        printf("Error while opening file\n");
        exit(-1);
    }
    for (size_t i = 0; i < matrixCount; i++)
    {
        M_Save(matrix[i], file); 
    }
    fclose(file);
    
}