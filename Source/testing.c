#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "../Headers/errors.h"
#include "../Headers/preAssembler.h"

int main()
{
    char *outputFileName = malloc(sizeof(char) * 9);
    executePreAssembler("test.txt", &outputFileName);
    printf("%s\n", outputFileName);
    
    return 0;
}

// gcc Source/dataStructers.c Source/errors.c Source/preAssembler.c Source/testing.c Source/utilities.c -o test