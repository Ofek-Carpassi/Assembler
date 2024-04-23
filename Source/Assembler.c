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

#define MAX_LINE_LENGTH 81

int checkOutput(char *fileName);

int main(int argc, char *argv[])
{
    int i;
    char *inputFile;
    char *preAssemblerOutput;
    char *firstPassOutput;
    location tmpLocation;
    /* We need to translate all the given files
    We need to make sure every file exists and ends with ".as" */
    if(argc < 2)
        printIntError(ERROR_CODE_1);
    for(i = 1; i < argc; i++)
    {
        /* Open the input file */
        inputFile = (char *)calloc(strlen(argv[i]) + 1, sizeof(char));
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
            free(tmpLocation.fileName);
            continue;
        }
        /* Allocate memory for the output files */
        preAssemblerOutput = (char *)malloc(strlen(inputFile));
        firstPassOutput = (char *)malloc(strlen(inputFile));
        if(preAssemblerOutput == NULL || firstPassOutput == NULL)
        {
            printIntError(ERROR_CODE_10);
            free(inputFile);
            free(preAssemblerOutput);
            free(firstPassOutput);
            free(tmpLocation.fileName);
            continue;
        }
        /* Execute the pre assembler and the first pass - the first pass will execute the second pass if no errors were found */
        executePreAssembler(inputFile, &preAssemblerOutput);
        executeFirstPass(preAssemblerOutput, &firstPassOutput);

        /* Free the allocated memory */
        free(preAssemblerOutput);
        free(firstPassOutput);
        free(inputFile);
    }

    printf("%d", checkOutput("test.ob"));

    return 0;
}

int checkOutput(char *fileName)
{
    FILE *outputFile = fopen(fileName, "r");
    FILE *exOuput = fopen("expectedOutput.txt", "r");
    char *line = (char *)calloc(MAX_LINE_LENGTH, sizeof(char)), *expectedLine = (char *)calloc(MAX_LINE_LENGTH, sizeof(char));
    if(outputFile == NULL)
        return 0;
    
    while(fgets(line, MAX_LINE_LENGTH, outputFile) != NULL)
    {
        if(fgets(expectedLine, MAX_LINE_LENGTH, exOuput) == NULL)
            return 0;
        if(strcmp(line, expectedLine) != 0)
            return 0;
    }
    if(fgets(expectedLine, MAX_LINE_LENGTH, exOuput) != NULL)
        return 0;

    return 1;
}