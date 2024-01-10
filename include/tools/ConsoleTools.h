#pragma once
#include <stdlib.h>

//Get the console width and height
void GetConsoleSize(int* width, int* height);

//Print a line of the console width
void PrintLine();

//Print a text centered on the console
void PrintCentered(const char* text);
