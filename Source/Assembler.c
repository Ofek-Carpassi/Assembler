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