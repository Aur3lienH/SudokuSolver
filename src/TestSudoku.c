#include "TestSudoku.h"
#include "Sudoku.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include "tools/TestTools.h"



int JPGfilter(const struct dirent* name)
{
    if(strstr(name->d_name,".jpeg") != NULL || strstr(name->d_name,".jpg") != NULL)
    {
        return 1;
    }
    return 0;
}



void TestSudoku(char* folder)
{
    struct dirent **namelist;
    int n = scandir(folder, &namelist, JPGfilter, alphasort);
    if (n == -1) {
        perror("scandir");
        exit(EXIT_FAILURE);
    }
    if(n == 0)
    {
        printf("No image found !\n");
        exit(-1);
    }
    for (size_t i = 0; i < n; i++)
    {
        char* path = malloc(sizeof(char) * 100);
        sprintf(path,"%s/%s",folder,namelist[i]->d_name);
        int success = 0;
        char* result = GetResolvedSudoku(path, &success);
        PrintTestResult(namelist[i]->d_name,success);
        free(path);
    }
}