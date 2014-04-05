#include "err.h"

#include <cstdarg>
#include <stdio.h>
#include <stdlib.h>

void err(bool exitProgram, const char *msg, ...)
{
    if(msg==NULL)
        fprintf(stderr, "error\n");
    else {
        fprintf(stderr, "error: ");
        va_list args;
        va_start (args, msg);
        char buffer[256];
        vsprintf (buffer, msg, args);
        fprintf(stderr, "%s\n",buffer);
        va_end (args);
    }
    if(exitProgram) exit(0);
}

