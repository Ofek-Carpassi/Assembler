/* Importing all needed libraries */
#include <stdio.h> /* File operations */
#include <stdlib.h> /* Memory allocation */
#include <string.h> /* String operations */
#include "../Headers/preAssembler.h" /* Function declarations */
#include "../Headers/errors.h" /* Error handling */
#include "../Headers/globalVariables.h" /* Global variables */
#include "../Headers/utilities.h" /* Utility functions */

/* Global variables - lineNumber in the source file, and the location in the source file */
int lineNumberSrc = 1;
location loc;

/* Function used to check if a line is a macro declaration */
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

/* Function used to get the name of the macro */
char *getMacroName(char *line)
{
    /* Clean the line from white spaces */
    char *cleanedLine = cleanLine(line);

    /* Create a buffer to store the name */
    char *name = (char *)calloc(strlen(cleanedLine) - 3, sizeof(char));
    if(name == NULL)
    {
        printIntError(ERROR_CODE_10);
        exit(1);
    }

    /* Get the name from the line (the line looks like "mcr <name>") */
    sscanf(cleanedLine, "%*s %s", name);
    free(cleanedLine);

    /* Return the name */
    return name;
}

/* Function used to save a macro to the macro list */
void saveMacroToList(char *file, Node **head, char *name)
{
    /* Initialize all needed variables */
    int i = 0, j = 0;
    /* Used to store the line, the macro definition, and the cleaned line */
    char *line, *macroDefinition, *cleanedLine;
    /* Open the source file */
    FILE *inputFile = fopen(file, "r");
    if (inputFile == NULL)
        printIntError(ERROR_CODE_11);

    /* Create a buffer to store the line */
    line = (char *)calloc(MAX_LINE_LENGTH, sizeof(char));
    if(line == NULL) {
        printIntError(ERROR_CODE_10);
        exit(1);
    }
    /* Skip all the lines until the lineNumberSrc+1 */
    for (i = 1; i <= lineNumberSrc; i++)
        fgets(line, MAX_LINE_LENGTH, inputFile);

    /* Create a buffer to store the macro definition */
    macroDefinition = (char *)calloc(1, sizeof(char));
    if(macroDefinition == NULL)
    {
        printIntError(ERROR_CODE_10);
        exit(1);
    }

    lineNumberSrc++;
    loc.line = lineNumberSrc;

    /* Loop through the file */
    while(fgets(line, MAX_LINE_LENGTH, inputFile) != NULL)
    {
        /* Clean the line from white spaces */
        cleanedLine = cleanLine(line);
        /* Check if the line is the end of the macro */
        if(strncmp(cleanedLine, "endmcr", 6) == 0)
        {
			if(cleanedLine[strlen(cleanedLine)-1] == '\n')
				cleanedLine[strlen(cleanedLine)-1] = '\0';
            /* If the line has more than 7 characters, print an error */
            if(strlen(cleanedLine) > 7)
                printExtError(ERROR_CODE_4, loc);

            free(cleanedLine);
            break;
        }

        /* Concatenate the line to the macro definition */
        macroDefinition = realloc(macroDefinition, strlen(macroDefinition) + strlen(cleanedLine) + 1);
        if(macroDefinition == NULL)
        {
            printIntError(ERROR_CODE_10);
            free(cleanedLine);
            exit(1);
        }
        strcat(macroDefinition, cleanedLine);

        /* Increment the line number */
        lineNumberSrc++;
        /* Update the location */
        loc.line = lineNumberSrc;

        free(cleanedLine);
    }

    /* Close the file */
    fclose(inputFile);

    /*
    Make sure the macro definition doesn't have any:
        - Empty lines (two new lines in a row)
        - Spaces at the beginning of the line
    */
    for(i = 0; (unsigned)i < strlen(macroDefinition); i++)
    {
        /* Remove the new lines and spaces */
        if(macroDefinition[i] == '\n' && macroDefinition[i+1] == '\n')
            /* Remove the new line */
            for(j = i; (unsigned)j < strlen(macroDefinition); j++)
                macroDefinition[j] = macroDefinition[j+1];
        /* Remove the spaces at the beginning of the line */
        if(macroDefinition[i] == ' ' && macroDefinition[i-1] == '\n')
            /* Remove the space */
            for(j = i; (unsigned)j < strlen(macroDefinition); j++)
                macroDefinition[j] = macroDefinition[j+1];
    }

    /* Remove the last new line */
    if(macroDefinition[strlen(macroDefinition) - 1] == '\n')
        macroDefinition[strlen(macroDefinition) - 1] = '\0';

    /* Create a new node containing the macro */
    addNode(head, name, macroDefinition, lineNumberSrc);

    /* Free the macro definition */
    free(macroDefinition);
    /* Free the line */
    free(line);
}

/* Function used to search for a node in the list */
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

/* Function to know if a line is a call to a macro */
int isCallToMacro(char *line, Node **head)
{
    /* Initialize all needed variables */
    int found = 0; /* Used to know if the macro is found */

    /* Check if the line has only one word */
    if(countWords(line) == 1)
    {
        /* Reset the found variable */
        found = 0;
        /* Make sure the lineCopy doesn't end with a new line */
        if(line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = '\0';
        /* Search for the macro in the list */
        searchNodeInList(*head, line, &found);
        /* If the macro was found, return true */
        if(found)
        {
            return 1;
        }
    }
    return 0;
}

/* Explained in the header file */
void executePreAssembler(char *file, char **outputFileName)
{
    /* Create the input and output files */
    FILE *inputFile, *outputFile;
    /* Strings to store the line and the cleaned line */
    char line[MAX_LINE_LENGTH] = "", *cleanedLine;
    /* found is used to know if the macro was found */
    int found = 0;
    /* Create a node and a linked list to store the macros */
    Node *node, *macroList = NULL;

    /* Open the input file */
    inputFile = fopen(file, "r");
    /* If the file doesn't exist, print an error and return */
    if (inputFile == NULL)
    {
        printIntError(ERROR_CODE_11);
        exit(1);
    }

    /* Create an output file with the same name but with the extension .am */
    *outputFileName = calloc(strlen(file)+1, sizeof(char));
    if(*outputFileName == NULL)
    {
        printIntError(ERROR_CODE_10);
        exit(1);
    }
    strcpy(*outputFileName, file);
    /* Replace the file from <name>.as to <name>.am */
    (*outputFileName)[strlen(*outputFileName) - 2] = 'a';
    (*outputFileName)[strlen(*outputFileName) - 1] = 'm';
    /* Open the output file */
    outputFile = fopen(*outputFileName, "w");
    if(outputFile == NULL)
    {
        printIntError(ERROR_CODE_13);
        exit(1);
    }

    /* Initialize the location */
    loc.fileName = file;

    /* Loop through the input file */
    while (fgets(line, MAX_LINE_LENGTH, inputFile) != NULL)
    {
        /* Check if the line is a comment */
        if(line[0] == ';')
        {
            lineNumberSrc++;
            continue;
        }

        /* Clean the line from white spaces */
        cleanedLine = cleanLine(line);
        

        if(cleanedLine == NULL)
        {
            printIntError(ERROR_CODE_45);
            exit(1);
        }
        /* Check if the line is a macro declaration */
        if (!isMacroDeclaration(cleanedLine))
        {
            /* Remove the new line from the end of the line */
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
                /* Remove the new line from the end of the line */
                if(cleanedLine[strlen(cleanedLine) - 1] == '\n')
                    cleanedLine[strlen(cleanedLine) - 1] = '\0';

                /* Print the line to the output file */
                if(lineNumberSrc != 1)
                    fprintf(outputFile, "\n%s", cleanedLine);
                else
                    fprintf(outputFile, "%s", cleanedLine);
            }
        }
        else
        {
            /* Save the original line number */
            int originalLineNumberSrc = lineNumberSrc, i = 0;
            /* Get the name of the macro */
            char *name = getMacroName(line);
            
            /* Check if the name is valid */
            if(!isValidMacroName(name))
            {
                printExtError(ERROR_CODE_3, loc);
            }

            /* Save the macro to the linked list */
            saveMacroToList(file, &macroList, name);
            
            /* Skip the lines of the macro */
            for(i = originalLineNumberSrc; i < lineNumberSrc; i++)
                fgets(line, MAX_LINE_LENGTH, inputFile);

            free(name);
        }

        /* Increment the line number */
        lineNumberSrc++;
        /* Update the location */
        loc.line = lineNumberSrc;
        free(cleanedLine);
    }

    /* Close the input and output files */
    fclose(inputFile);
    fclose(outputFile);

    /* Free the macro list */
    freeList(macroList);
}

