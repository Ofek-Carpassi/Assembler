#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "../Headers/errors.h"
#include "../Headers/preAssembler.h"
#include "../Headers/firstPass.h"
#include "../Headers/dataStructures.h"
#include "../Headers/utilities.h"

int main()
{   
    char *inputFile = "test.txt";
    char *preAssemblerOutput = (char *)malloc(strlen(inputFile));
    char *firstPassOutput = (char *)malloc(strlen(inputFile));
    executePreAssembler("test.txt", &preAssemblerOutput);
    printf("%s\n", preAssemblerOutput);
    executeFirstPass(preAssemblerOutput, &firstPassOutput);
    printf("%s\n", firstPassOutput);
    
    return 0;
}

/* gcc Source/dataStructers.c Source/errors.c Source/preAssembler.c Source/testing.c Source/utilities.c Source/firstPass.c -Wall -ansi -pedantic -o test */