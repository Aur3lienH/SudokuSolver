#pragma once


int** ImageToSudoku(char* path);

int** GetResolvedSudoku(char* path, int* success);

int getErrorSudoku(int grid[9][9], int** errorFound);