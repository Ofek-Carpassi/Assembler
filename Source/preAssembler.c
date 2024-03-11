#include <stdio.h> // Used for fprintf, fopen, fclose, fgets, printf...
#include <stdlib.h> // Used for malloc, free...
#include <string.h> // Used for strlen, strcpy...
#include "../Headers/preAssembler.h" // This file contains the execution of the pre-assembler
#include "../Headers/errors.h" // Used to print errors
#include "../Headers/globalVariables.h" // Used to get the maximum line length
#include "../Headers/utilities.h" // Used to clean the line

/* Explained in the header file */
void executePreAssembler(char *file, char *outputFileName[])
{
    /* Open the input file */
    FILE *inputFile = fopen(file, "r");
    /* If the file doesn't exist, print an error and return */
    if (inputFile == NULL)
    {
        printIntError(ERROR_CODE_11);
    }

    /* Create the output file name */
    char outputName[strlen(file) + 1];
    /* Copy the input file name to the output file name and change the ending to .am */
    strcpy(outputName, file);

    /* Find the dot in the file name */
    int dotIndex = 0;
    while (outputName[dotIndex] != '.')
    {
        dotIndex++;
    }

    /* Change the ending to .am (after the dot there is .txt, so we need to replace it with .am) */
    outputName[dotIndex++] = '.';
    outputName[dotIndex++] = 'a';
    outputName[dotIndex++] = 'm';
    outputName[dotIndex++] = '\0';

    /* Allocate memory for the output file name and copy the output name to it */
    *outputFileName = (char *)malloc(sizeof(char) * (strlen(outputName) + 1));
    strcpy(*outputFileName, outputName);

    /* Open the output file (create it with write permissions) */
    FILE *outputFile = fopen(outputName, "w");
    /* If the file creation fails, print an error and return */
    if (outputFile == NULL)
    {
        printIntError(ERROR_CODE_13);
    }

    /* Create a buffer to store the line */
    char line[MAX_LINE_LENGTH] = "";

    /* Loop through the input file */
    while (fgets(line, MAX_LINE_LENGTH, inputFile) != NULL)
    {
        /* Clean the line */
        char *cleanedLine = cleanLine(line);
        /* Print the cleaned line to the output file */
        fprintf(outputFile, "%s", cleanedLine);
        /* Free the memory allocated for the cleaned line */
        free(cleanedLine);
        // add a new line character to the end of the line
        //fprintf(outputFile, "\n");
    }
}