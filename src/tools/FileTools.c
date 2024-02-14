#include "tools/FileTools.h"
#include <stdlib.h>
#include <err.h>
#include <errno.h>



void CheckRead(int read)
{
    if (read == -1)
    {
        errx(1,"Error while reading file");
        exit(EXIT_FAILURE);
    }
}