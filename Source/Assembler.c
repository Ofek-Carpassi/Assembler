#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "../Headers/errors.h"
#include "../Headers/preAssembler.h"
#include "../Headers/firstPass.h"
#include "../Headers/dataStructures.h"
#include "../Headers/utilities.h"
#include "../Headers/secondPass.h"

int main(int argc, char *argv[])
{
    int i;
    char *inputFile;
    char *preAssemblerOutput;
    char *firstPassOutput;
    /* We need to translate all the given files
    We need to make sure every file exists and ends with ".as" */
    if(argc < 2)
        printIntError(ERROR_CODE_1);
    for(i = 1; i < argc; i++)
    {
        inputFile = (char *)calloc(strlen(argv[i]) + 1, sizeof(char));
        if(inputFile == NULL)
            printIntError(ERROR_CODE_10);
        strcpy(inputFile, argv[i]);
        if(strstr(inputFile, ".as") == NULL)
            printIntError(ERROR_CODE_52);
        preAssemblerOutput = (char *)malloc(strlen(inputFile));
        firstPassOutput = (char *)malloc(strlen(inputFile));
        executePreAssembler(inputFile, &preAssemblerOutput);
        executeFirstPass(preAssemblerOutput, &firstPassOutput);

        free(preAssemblerOutput);
        free(firstPassOutput);
        free(inputFile);
    }
    return 0;
}