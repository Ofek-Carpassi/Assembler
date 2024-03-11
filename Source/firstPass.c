#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/firstPass.h"
#include "../Headers/errors.h"
#include "../Headers/globalVariables.h"
#include "../Headers/secondPass.h"

int IC = 0, DC = 0; // Initialize the instruction counter and the data counter

void executeFirstPass(char *file, char **outputFileName)
{
    FILE *inputFile = fopen(file, "r"); // Open the input file

    if (inputFile == NULL) // If the file doesn't exist
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

    char line[MAX_LINE_LENGTH] = ""; // Create a buffer to store the line

    while(fgets(line, MAX_LINE_LENGTH, inputFile) != NULL) // Loop through the input file
    {
        char *cleanedLine = cleanLine(line); // Clean the line
        fprintf(outputFile, "%s\n", cleanedLine); // Print the cleaned line to the output file
        free(cleanedLine); // Free the memory of the cleaned line
        
    }
}