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


ProgressBar* PB_Create(size_t count);

void PB_ChangeProgress(ProgressBar* progressBar,size_t count, float loss);


