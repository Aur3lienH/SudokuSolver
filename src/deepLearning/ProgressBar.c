#include "ProgressBar.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef WIN32

#include <windows.h>

#else

#include <sys/ioctl.h>
#include <unistd.h>

#endif

int GetConsoleWidth()
{
#ifdef WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
#else
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
#endif
}


void PB_DrawBar(const int size, float progress)
{
    int newProgress = (int) (progress * size);
    printf("[");
    for (int i = 0; i < size - 2; i++)
    {
        if (i < newProgress)
            printf("=");
        else if (i == newProgress)
            printf(">");
        else
            printf(" ");
    }
    printf("]");
}
ProgressBar* PB_Create(size_t count)
{
    ProgressBar* progressBar = (ProgressBar*)malloc(sizeof(ProgressBar));
    progressBar->lastTime = time(NULL);
    progressBar->startedTime = time(NULL);
    progressBar->lastProgress = 0;
    progressBar->max = count;
    return progressBar;
}

size_t PrintDuration(time_t time)
{
    int hours = time / 3600;
    int minutes = (time % 3600) / 60;
    int seconds = (time % 3600) % 60;
    return printf("%02d:%02d:%02d", hours, minutes, seconds);
}

void PB_ChangeProgress(ProgressBar* progressBar,size_t count, float loss)
{
    printf("\r");

    float newProgress = count / (float)progressBar->max;
    int consoleSize = GetConsoleWidth();
    
    int sizeUsed = 0;

    time_t timePassed = time(NULL) - progressBar->startedTime;
    time_t timeRemaining = (progressBar->max - count) * timePassed / (count+1);

    progressBar->lastTime = time(NULL);
    progressBar->lastProgress = newProgress;


    sizeUsed += PrintDuration(timePassed);
    sizeUsed += printf(" / ");
    sizeUsed += PrintDuration(timeRemaining);
    sizeUsed += printf(" | ");
    sizeUsed += printf("Loss : %e ",loss);
    sizeUsed += printf(" | ");
    PB_DrawBar(consoleSize - sizeUsed, newProgress);

    fflush(stdout);
}