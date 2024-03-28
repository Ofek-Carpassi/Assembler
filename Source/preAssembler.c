#include <stdio.h> // Used for fprintf, fopen, fclose, fgets, printf...
#include <stdlib.h> // Used for malloc, free...
#include <string.h> // Used for strlen, strcpy...
#include "../Headers/preAssembler.h" // This file contains the execution of the pre-assembler
#include "../Headers/errors.h" // Used to print errors
#include "../Headers/globalVariables.h" // Used to get the maximum line length
#include "../Headers/utilities.h" // Used to clean the line

int isMacroDeclaration(char *line)
{
    /* Check if the first word in the line is "mcr" */
    if (strncmp(line, "mcr", 3) == 0)
    {
        return 1;
    }
    return 0;
}

char *getMacroName(char *line)
{
    /* Create a buffer to store the name */
    char *name = malloc(sizeof(char) * MAX_LINE_LENGTH);
    /* Get the second word in the line */
    sscanf(line, "%*s %s", name);
    /* Return the name */
    return name;
}

int saveMacroToList(char *file, Node **head, int lineNumber, char *name)
{
    /* Open the file */
    FILE *inputFile = fopen(file, "r");

    /* If the file doesn't exist, print an error and return */
    if (inputFile == NULL)
    {
        printIntError(ERROR_CODE_11);
    }

    /* Skip all the lines until the lineNumber+1 */
    for (int i = 1; i <= lineNumber; i++)
    {
        char line[MAX_LINE_LENGTH] = "";
        fgets(line, MAX_LINE_LENGTH, inputFile);
    }

    /* Create a buffer to store the line */
    char line[MAX_LINE_LENGTH] = "";
    char *macroDefinition = calloc(1, sizeof(char));
    if(macroDefinition == NULL)
    {
        printIntError(ERROR_CODE_10);
    }

    /* Loop through the file */
    while(fgets(line, MAX_LINE_LENGTH, inputFile) != NULL && strncmp(line, "endmcr", 6) != 0)
    {
        char *cleanedLine = cleanLine(line);

        macroDefinition = realloc(macroDefinition, strlen(macroDefinition) + strlen(cleanedLine) + 1);
        if(macroDefinition == NULL)
        {
            printIntError(ERROR_CODE_10);
        }

        /* Save the line to the linked list */
        strcat(macroDefinition, cleanedLine);
        strcat(macroDefinition, "\n");

        /* Free the cleaned line */
        free(cleanedLine);

        /* Increment the line number */
        lineNumber++;
    }

    /* Close the file */
    fclose(inputFile);

    /* Create a new node */
    addNode(head, name, macroDefinition, lineNumber);

    /* Return 1 */
    return lineNumber;
}

int isValidMacroName(char *name)
{
    /* Create an array of invalid names */
    char *invalidNames[] = {"mcr", "endmcr", ".data", ".string", ".entry", ".extern", "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop", "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
    /* Loop through the array */
    for (int i = 0; i < 27; i++)
    {
        /* If the name is invalid, return 0 */
        if (strcmp(name, invalidNames[i]) == 0)
        {
            return 0;
        }
    }
    /* Return 1 */
    return 1;
}

int isCallToMacro(char *line, Node **head)
{
    /* Check if the single word in the line is a macro name */
    if(countWords(line) == 1)
    {
        /* Remove the space at the end of the line */
        line[strlen(line) - 1] = '\0';
        int found = 0;
        Node *node = searchNodeInList(*head, line, &found);
        if(found)
        {
            return 1;
        }
    }
    return 0;
}

/* Explained in the header file */
void executePreAssembler(char *file, char *outputFileName[])
{
    /* Create a linked list to store the macros */
    Node *macroList = NULL;
    /* Create a variable to store the line number */
    int lineNumber = 1;

    /* Open the input file */
    FILE *inputFile = fopen(file, "r");
    /* If the file doesn't exist, print an error and return */
    if (inputFile == NULL)
    {
        printIntError(ERROR_CODE_11);
    }

    /* Create an output file with the same name but with the extension .am */
    *outputFileName = malloc(sizeof(char) * (strlen(file) + 1));
    strcpy(*outputFileName, file);
    /* Replace the last character with 'm' */
    (*outputFileName)[strlen(file) - 1] = '\0';
    /* Replace the last character with 'm' */
    (*outputFileName)[strlen(file) - 2] = 'm';
    /* Replace the second to last character with 'a' */
    (*outputFileName)[strlen(file) - 3] = 'a';
    /* Open the output file */
    FILE *outputFile = fopen(*outputFileName, "w");

    /* Create a buffer to store the line */
    char line[MAX_LINE_LENGTH] = "";

    /* Loop through the input file */
    while (fgets(line, MAX_LINE_LENGTH, inputFile) != NULL)
    {
        printf("Line: %s\n", line);
        /* Clean the line */
        char *cleanedLine = cleanLine(line);
        /* Print the cleaned line to the output file */
        if (!isMacroDeclaration(cleanedLine))
        {
            if(isCallToMacro(cleanedLine, &macroList) == 1)
            {
                /* Replace the macro call with the macro's definition */
                int found = 0;
                Node *node = searchNodeInList(macroList, cleanedLine, &found);
                fprintf(outputFile, "%s", node->data);
            }
            else
            {
                fprintf(outputFile, "%s\n", cleanedLine);
            }
        }
        else
        {
            /* Get the name of the macro */
            char *name = getMacroName(cleanedLine);
            /* Check if the name is valid */
            if (!isValidMacroName(name))
            {
                printIntError(ERROR_CODE_3);
            }
            /* Save the macro to the linked list */
            lineNumber = saveMacroToList(file, &macroList, lineNumber, name);

            /* Skip the lines of the macro */
            for (int i = 1; i <= lineNumber; i++)
            {
                fgets(line, MAX_LINE_LENGTH, inputFile);
            }
        }

        free(cleanedLine);
        lineNumber++;
    }
}