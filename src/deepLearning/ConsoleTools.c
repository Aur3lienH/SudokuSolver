#include "ConsoleTools.h"
#if WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void GetConsoleSize(int* width, int* height)
{
#if WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    *width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    *width = w.ws_col;
    *height = w.ws_row;
#endif
}

void PrintLine()
{
    int width, height;
    GetConsoleSize(&width, &height);
    for (int i = 0; i < width; i++)
    {
        printf("-");
    }
    printf("\n");
}


void PrintCentered(const char* text)
{
    int width, height;
    GetConsoleSize(&width, &height);
    int textLength = strlen(text);
    int x = (width - textLength) / 2;
    for (int i = 0; i < x; i++)
    {
        printf(" ");
    }
    printf("%s\n", text);
}