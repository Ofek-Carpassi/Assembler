#include <stdio.h> // Used for fprintf, fopen, fclose, fgets, printf...
#include <stdlib.h> // Used for malloc, free...
#include <string.h> // Used for strlen, strcpy...
#include "../Headers/preAssembler.h" // This file contains the execution of the pre-assembler
#include "../Headers/errors.h" // Used to print errors
#include "../Headers/globalVariables.h" // Used to get the maximum line length
#include "../Headers/utilities.h" // Used to clean the line

int isMacroDeclaration(char *line)
{
    /* Clean the line from white spaces */
    char *cleanedLine = cleanLine(line);
    /* Check if the first word in the line is "mcr" */
    if (strncmp(cleanedLine, "mcr", 3) == 0)
    {
        /* Check if the line has exactly 2 words */
        if(countWords(cleanedLine) != 2)
            printIntError(ERROR_CODE_4); /* Print an error */
        /* Free the cleaned line */
        free(cleanedLine);
        return 1;
    }
    /* Free the cleaned line and return false */
    free(cleanedLine);
    return 0;
}

char *getMacroName(char *line)
{
    /* Clean the line from white spaces */
    char *cleanedLine = cleanLine(line);

    /* Create a buffer to store the name */
    char *name = calloc(MAX_LINE_LENGTH, sizeof(char));
    if(name == NULL)
    {
        printIntError(ERROR_CODE_10);
    }

    /* Get the name from the line (the line looks like "mcr <name>") */
    sscanf(cleanedLine, "%*s %s", name);
    free(cleanedLine);

    /* Return the name */
    return name;
}

void saveMacroToList(char *file, Node **head, int*lineNumber, char *name)
{
    /* Open the file */
    FILE *inputFile = fopen(file, "r");
    if (inputFile == NULL)
        printIntError(ERROR_CODE_11);

    /* Create a buffer to store the line */
    char *line = calloc(MAX_LINE_LENGTH, sizeof(char));
    if(line == NULL)
        printIntError(ERROR_CODE_10);

    /* Skip all the lines until the lineNumber+1 */
    for (int i = 1; i <= *lineNumber; i++)
        fgets(line, MAX_LINE_LENGTH, inputFile);

    /* Create a buffer to store the macro definition */
    char *macroDefinition = calloc(1, sizeof(char));
    if(macroDefinition == NULL)
        printIntError(ERROR_CODE_10);

    /* Loop through the file */
    while(fgets(line, MAX_LINE_LENGTH, inputFile) != NULL)
    {
        /* Clean the line from white spaces */
        char *cleanedLine = cleanLine(line);

        /* Check if the line is the end of the macro */
        if(strncmp(cleanedLine, "endmcr", 6) == 0)
        {
            /* If the line has more than 7 characters, print an error */
            if(strlen(cleanedLine) > 7)
                printIntError(ERROR_CODE_4);
            break;
        }

        /* Add a tab at the beginning of the line */
        char *tab = calloc(2, sizeof(char));
        if(tab == NULL)
            printIntError(ERROR_CODE_10);
        tab[0] = '\t';
        tab[1] = '\0';

        /* Reallocate the macro definition */
        macroDefinition = realloc(macroDefinition, strlen(macroDefinition) + strlen(tab) + strlen(cleanedLine) + 1);
        if(macroDefinition == NULL)
            printIntError(ERROR_CODE_10);

        /* Concatenate the tab and the cleaned line to the macro definition */
        strcat(macroDefinition, tab);
        strcat(macroDefinition, cleanedLine);

        /* Free the tab and the cleaned line */
        free(tab);
        free(cleanedLine);

        /* Increment the line number */
        lineNumber++;
    }

    /* Close the file */
    fclose(inputFile);

    /* Loop through the macro definition */
    for(int i = 0; i < strlen(macroDefinition); i++)
    {
        /* Remove the new lines and spaces */
        if(macroDefinition[i] == '\n' && macroDefinition[i+1] == '\n')
        {
            // Remove the new line
            for(int j = i; j < strlen(macroDefinition); j++)
                macroDefinition[j] = macroDefinition[j+1];
        }
        /* Remove the spaces at the beginning of the line */
        if(macroDefinition[i] == ' ' && macroDefinition[i-1] == '\n')
        {
            // Remove the space
            for(int j = i; j < strlen(macroDefinition); j++)
                macroDefinition[j] = macroDefinition[j+1];
        }
    }

    /* Create a new node containing the macro */
    addNode(head, name, macroDefinition, *lineNumber);
}

int isValidMacroName(char *name)
{
    /* Create an array of invalid names */
    char *invalidNames[] = {"mcr", "endmcr", ".data", ".string", ".entry", ".extern", "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop", "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
    /* Loop through the array */
    for (int i = 0; i < 27; i++)
        /* If the name is invalid, return 0 */
        if (strcmp(name, invalidNames[i]) == 0)
            return 0;
    /* Return 1 */
    return 1;
}

int isCallToMacro(char *line, Node **head)
{
    /* Clean the line from white spaces */
    char *cleanedLine = cleanLine(line);

    /* Check if the single word in the line is a macro name */
    if(countWords(cleanedLine) == 1)
    {
        /* Check if the line is a macro call */
        int found = 0;
        /* Remove the last character from the line */
        cleanedLine[strlen(cleanedLine) - 1] = '\0';
        /* Search for the macro in the list */
        Node *node = searchNodeInList(*head, cleanedLine, &found);
        /* If the macro is found, return true */
        if(found)
        {
            free(cleanedLine);
            return 1;
        }
    }
    free(cleanedLine);
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
        printIntError(ERROR_CODE_11);

    /* Create an output file with the same name but with the extension .am */
    *outputFileName = calloc(strlen(file) + 1, sizeof(char));
    if(*outputFileName == NULL)
        printIntError(ERROR_CODE_10);
    strcpy(*outputFileName, file);
    /* Replace the file from <name>.txt\0 to <name>.am\0 */
    (*outputFileName)[strlen(file) - 3] = 'a';
    (*outputFileName)[strlen(file) - 2] = 'm';
    (*outputFileName)[strlen(file) - 1] = '\0';
    /* Open the output file */
    FILE *outputFile = fopen(*outputFileName, "w");

    /* Create a buffer to store the line */
    char line[MAX_LINE_LENGTH] = "";

    /* Loop through the input file */
    while (fgets(line, MAX_LINE_LENGTH, inputFile) != NULL)
    {
        /* Check if the line is a macro declaration */
        if (!isMacroDeclaration(line))
        {
            /* If the line isn't a macro declaration, check if it's a macro call */
            if(isCallToMacro(line, &macroList) == 1)
            {
                /* Replace the macro call with the macro's definition */
                int found = 0;
                char *cleanedLine = cleanLine(line);
                cleanedLine[strlen(cleanedLine) - 1] = '\0';
                Node *node = searchNodeInList(macroList, cleanedLine, &found);
                /* If the macro is found, print the macro's definition */
                if(found)
                    fprintf(outputFile, "%s", node->data);
                /* Free the cleaned line */
                free(cleanedLine);
            }
            else
                /* If the line isn't a macro declaration or a macro call, print the line */
                fprintf(outputFile, "%s", line);
        }
        else
        {
            /* Get the name of the macro */
            char *name = getMacroName(line);
            /* Check if the name is valid */
            if (!isValidMacroName(name))
                printIntError(ERROR_CODE_3);
            /* Save the macro to the linked list */
            saveMacroToList(file, &macroList, &lineNumber, name);
            /* Skip the lines of the macro */
            for (int i = 1; i <= lineNumber; i++)
                fgets(line, MAX_LINE_LENGTH, inputFile);
        }
        /* Increment the line number */
        lineNumber++;
    }
}