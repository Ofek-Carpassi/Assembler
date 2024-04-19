#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include "../Headers/preAssembler.h" 
#include "../Headers/errors.h" 
#include "../Headers/globalVariables.h"
#include "../Headers/utilities.h"

int lineNumberSrc = 1;
location loc;

int isMacroDeclaration(char *line)
{
    /* Check if the first word in the line is "mcr" */
    if (strncmp(line, "mcr", 3) == 0)
    {
        /* Check if the line has exactly 2 words */
        if(countWords(line) < 2)
            printExtError(ERROR_CODE_15, loc);
        else if(countWords(line) > 2)
            printExtError(ERROR_CODE_16, loc);
        return 1;
    }
    return 0;
}

char *getMacroName(char *line)
{
    /* Clean the line from white spaces */
    char *cleanedLine = cleanLine(line);

    /* Create a buffer to store the name */
    char *name = (char *)calloc(strlen(cleanedLine) - 3, sizeof(char));
    if(name == NULL)
        printIntError(ERROR_CODE_10);

    /* Get the name from the line (the line looks like "mcr <name>") */
    sscanf(cleanedLine, "%*s %s", name);
    free(cleanedLine);

    /* Return the name */
    return name;
}

int saveMacroToList(char *file, Node **head, int lineNumberSrc, char *name)
{
    int i = 0;
    int j = 0;
    char *line;
    char *macroDefinition;
    char *cleanedLine;
    /* Open the file */
    FILE *inputFile = fopen(file, "r");
    if (inputFile == NULL)
        printIntError(ERROR_CODE_11);

    /* Create a buffer to store the line */
    line = calloc(MAX_LINE_LENGTH, sizeof(char));
    if(line == NULL)
        printIntError(ERROR_CODE_10);

    /* Skip all the lines until the lineNumberSrc+1 */
    for (i = 1; i <= lineNumberSrc; i++)
        fgets(line, MAX_LINE_LENGTH, inputFile);

    /* Create a buffer to store the macro definition */
    macroDefinition = calloc(1, sizeof(char));
    if(macroDefinition == NULL)
        printIntError(ERROR_CODE_10);

    /* Loop through the file */
    while(fgets(line, MAX_LINE_LENGTH, inputFile) != NULL)
    {
        /* Clean the line from white spaces */
        cleanedLine = cleanLine(line);

        /* Check if the line is the end of the macro */
        if(strncmp(cleanedLine, "endmcr", 6) == 0)
        {
            /* If the line has more than 7 characters, print an error */
            if(strlen(cleanedLine) > 7)
                printExtError(ERROR_CODE_4, loc);
            break;
        }

        /* Concatenate the line to the macro definition */
        macroDefinition = realloc(macroDefinition, strlen(macroDefinition) + strlen(cleanedLine) + 1);
        if(macroDefinition == NULL)
            printIntError(ERROR_CODE_10);
        strcat(macroDefinition, cleanedLine);

        /* Increment the line number */
        lineNumberSrc++;
    }

    free(cleanedLine);
    free(line);

    /* Close the file */
    fclose(inputFile);

    /* Loop through the macro definition */
    for(i = 0; i < strlen(macroDefinition); i++)
    {
        /* Remove the new lines and spaces */
        if(macroDefinition[i] == '\n' && macroDefinition[i+1] == '\n')
            /* Remove the new line */
            for(j = i; j < strlen(macroDefinition); j++)
                macroDefinition[j] = macroDefinition[j+1];
        /* Remove the spaces at the beginning of the line */
        if(macroDefinition[i] == ' ' && macroDefinition[i-1] == '\n')
            /* Remove the space */
            for(j = i; j < strlen(macroDefinition); j++)
                macroDefinition[j] = macroDefinition[j+1];
    }

    if(macroDefinition[strlen(macroDefinition) - 1] == '\n')
        macroDefinition[strlen(macroDefinition) - 1] = '\0';

    /* Create a new node containing the macro */
    addNode(head, name, macroDefinition, lineNumberSrc);

    return lineNumberSrc;
}

int isValidMacroName(char *name)
{
    /* Create an array of invalid names */
    char *invalidNames[] = {"mcr", "endmcr", ".data", ".string", ".entry", ".extern", "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop", "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
    int i = 0;
    /* Loop through the array */
    for (i = 0; i < 27; i++)
        /* If the name is invalid, return 0 */
        if (strcmp(name, invalidNames[i]) == 0)
            return 0;
    /* Return 1 */
    return 1;
}

int isCallToMacro(char *line, Node **head)
{
    /* Clean the line from white spaces */
    int found = 0;
    char *lineCopy = calloc(strlen(line) + 1, sizeof(char));
    if(lineCopy == NULL)
        printIntError(ERROR_CODE_10);

    strcpy(lineCopy, line);

    /* Check if the single word in the line is a macro name */
    if(countWords(lineCopy) == 1)
    {
        /* Check if the lineCopy is a macro call */
        found = 0;
        /* Make sure the lineCopy doesn't end with a new lineCopy */
        if(lineCopy[strlen(lineCopy) - 1] == '\n')
            lineCopy[strlen(lineCopy) - 1] = '\0';
        /* Search for the macro in the list */
        searchNodeInList(*head, lineCopy, &found);
        /* If the macro is found, return true */
        if(found)
        {
            free(lineCopy);
            return 1;
        }
    }
    free(lineCopy);
    return 0;
}

/* Explained in the header file */
void executePreAssembler(char *file, char **outputFileName)
{
    /* Create a linked list to store the macros */
    FILE *inputFile;
    FILE *outputFile;
    char line[MAX_LINE_LENGTH] = "";
    char *cleanedLine;
    int found = 0;
    Node *node;
    Node *macroList = NULL;

    /* Open the input file */
    inputFile = fopen(file, "r");
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
    outputFile = fopen(*outputFileName, "w");

    loc.fileName = file;

    /* Loop through the input file */
    while (fgets(line, MAX_LINE_LENGTH, inputFile) != NULL)
    {
        if(line[0] == ';')
        {
            lineNumberSrc++;
            continue;
        }

        cleanedLine = cleanLine(line);

        if(cleanedLine == NULL)
            printIntError(ERROR_CODE_10);
        /* Check if the line is a macro declaration */
        if (!isMacroDeclaration(cleanedLine))
        {
            if(cleanedLine[strlen(cleanedLine) - 1] == '\n')
                cleanedLine[strlen(cleanedLine) - 1] = '\0';
            /* If the line isn't a macro declaration, check if it's a macro call */
            if(isCallToMacro(cleanedLine, &macroList) == 1)
            {
                /* Replace the macro call with the macro's definition */
                found = 0;
                if(cleanedLine[strlen(cleanedLine) - 1] == '\n')
                    cleanedLine[strlen(cleanedLine) - 1] = '\0';
                node = searchNodeInList(macroList, cleanedLine, &found);
                /* If the macro is found, print the macro's definition */
                if(found)
                    fprintf(outputFile, "\n%s", node->data);
            }
            else
            {
                int i = 0;
                for(i = 0; i < strlen(cleanedLine); i++)
                    if(cleanedLine[i] == '\n')
                        cleanedLine[i] = '\0';

                if(lineNumberSrc != 1)
                    fprintf(outputFile, "\n%s", cleanedLine);
                else
                    fprintf(outputFile, "%s", cleanedLine);
            }
        }
        else
        {
            int newlineNumberSrc;
            /* Get the name of the macro */
            char *name = getMacroName(line);

            /* Check if the name is valid */
            if(!isValidMacroName(name))
                printIntError(ERROR_CODE_3);
            /* Save the macro to the linked list */
            newlineNumberSrc = saveMacroToList(file, &macroList, lineNumberSrc, name);
            /* Skip the lines of the macro */
            for(; lineNumberSrc <= newlineNumberSrc; lineNumberSrc++)
                fgets(line, MAX_LINE_LENGTH, inputFile);
        }

        /* Increment the line number */
        lineNumberSrc++;
        loc.line = lineNumberSrc;
        free(cleanedLine);
    }

    /* Close the input and output files */
    fclose(inputFile);
    fclose(outputFile);

    /* Free the macro list */
    freeList(macroList);
}