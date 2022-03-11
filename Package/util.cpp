#include "util.h"
#include <stdio.h>
#include <stdlib.h>

void errif(bool flag, const char* msg)
{
    if(flag)
    {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}

