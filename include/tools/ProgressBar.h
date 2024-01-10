#pragma once
#include <string.h>
#include <time.h>


typedef struct 
{
    size_t max;
    float lastProgress;

    time_t lastTime;
    time_t startedTime;
} ProgressBar;

//Create a progress bar
ProgressBar* PB_Create(size_t count);

//Change the progression of the progress bar
void PB_ChangeProgress(ProgressBar* progressBar,size_t count, float loss);


