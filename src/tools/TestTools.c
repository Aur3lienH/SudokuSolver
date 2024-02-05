#include "tools/TestTools.h"
#include <stdio.h>

void PrintTestResult(char* testName, int result)
{
    printf("%s : %s\n", testName, result ? "\033[1;32m[SUCCEED]\033[0m   " : "\033[1;31m[FAIL]\033[0m   ");
}