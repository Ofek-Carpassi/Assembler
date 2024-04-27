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
    location tmpLocation;
    char *inputFile, *preAssemblerOutput, *firstPassOutput;
    /* We need to translate all the given files
    We need to make sure every file exists and ends with ".as" */
    if(argc < 2)
        printIntError(ERROR_CODE_1);
    for(i = 1; i < argc; i++)
    {
        /* Open the input file */
        inputFile = (char *)calloc(strlen(argv[i]) + 2, sizeof(char));
        if(inputFile == NULL)
        {
            printIntError(ERROR_CODE_10);
            exit(1);
        }
        strcpy(inputFile, argv[i]);
        tmpLocation.fileName = inputFile;
        tmpLocation.line = 0;
        if(strstr(inputFile, ".as") == NULL)
        {
            printExtError(ERROR_CODE_8, tmpLocation);
            free(inputFile);
            continue;
        }
        /* Execute the pre assembler and the first pass - the first pass will execute the second pass if no errors were found */
        executePreAssembler(inputFile, &preAssemblerOutput);
        executeFirstPass(preAssemblerOutput, &firstPassOutput);

        /* Free the allocated memory */
        free(preAssemblerOutput);
        free(inputFile);
        free(firstPassOutput);
    }

    return 0;
}
