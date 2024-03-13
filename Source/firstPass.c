#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/firstPass.h"
#include "../Headers/errors.h"
#include "../Headers/globalVariables.h"
#include "../Headers/utilities.h"

int IC = 0, DC = 0; // Initialize the instruction counter and the data counter

const char *instructionNames[] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"}; // Array of the instruction names

int isInstruction(char *word)
{
    for (int i = 0; i < 16; i++)
    {
        if (strcmp(word, instructionNames[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

int checkLineType(char *line)
{
    if (line[0] == ';' || line[0] == '\n') // If the line is a comment
    {
        return COMMENT;
    }
    char *parsedLine[4] = {"", "", "", ""}; // Create an array to store the parsed line
    parseLine(line, parsedLine);
    if(strcmp(parsedLine[0], ".define") == 0)
    {
        return CONSTANT;
    }
    else if(strcmp(parsedLine[0], ".data") == 0)
    {
        return DATA;
    }
    else if(strcmp(parsedLine[0], ".string") == 0)
    {
        return STRING;
    }
    else if(strcmp(parsedLine[0], ".entry") == 0)
    {
        return ENTRY;
    }
    else if(strcmp(parsedLine[0], ".extern") == 0)
    {
        return EXTERN;
    }
    else if(parsedLine[0][strlen(parsedLine[0]) - 1] == ':')
    {
        return LABEL;
    }
    else if (isInstruction(parsedLine[0]))
    {
        return INSTRUCTION;
    }
    else
    {
        return ERROR;
    }
}

void executeFirstPass(char *file, char **outputFileName)
{
    FILE *inputFile = fopen(file, "r"); // Open the input file

    if (inputFile == NULL) // If the file doesn't exist
    {
        printIntError(ERROR_CODE_11);
    }

    /* Create the output file name */
    char outputName[strlen(file) + 2];
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
    outputName[dotIndex++] = 'o';
    outputName[dotIndex++] = 'b';
    outputName[dotIndex++] = 'j';
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
        printf("Didn't crash\n");
        int lineType = checkLineType(line); // Check the type of the line
        printf("Line type: %d\n", lineType);
        // Print the cleaned line to the output file
    }
}