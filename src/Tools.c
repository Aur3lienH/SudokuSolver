#include "Tools.h"

int CompareStrings(char* a, char* b)
{
    while(*a != '\0' && *b != '\0' && *a == *b)
    {
        a++;
        b++;
    }
    if(*a == '\0' && *b == *a)
    {
        return 1;
    }
    return 0;
}
