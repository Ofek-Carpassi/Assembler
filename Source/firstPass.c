#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../Headers/firstPass.h"
#include "../Headers/errors.h"
#include "../Headers/globalVariables.h"
#include "../Headers/utilities.h"
#include "../Headers/dataStructers.h"

int IC = 0, DC = 0; // Initialize the instruction counter and the data counter
int isLabel = 0; // Initialize the isLabel flag
const char *instructionNames[] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"}; // Array of the instruction names
Node *symbolTable = NULL; // Create the symbol table

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
    int lineType = 0;
    if (line[0] == ';' || line[0] == '\n') // If the line is a comment
    {
        lineType = COMMENT;
    }
    char *parsedLine[4] = {"", "", "", ""}; // Create an array to store the parsed line
    parseLine(line, parsedLine);
    if(strcmp(parsedLine[0], ".define") == 0)
    {
        handleConstant(line, &symbolTable);
    }
    else if(strcmp(parsedLine[0], ".data") == 0)
    {
        lineType = DATA;
        char *binaryLine = handleData(line, &symbolTable);
    }
    else if(strcmp(parsedLine[0], ".string") == 0)
    {
        lineType = STRING;
        char *binaryLine = handleString(line, &symbolTable);
    }
    else if(strcmp(parsedLine[0], ".entry") == 0)
    {
        lineType = ENTRY;
    }
    else if(strcmp(parsedLine[0], ".extern") == 0)
    {
        lineType = EXTERN;
    }
    else if(parsedLine[0][strlen(parsedLine[0]) - 1] == ':')
    {
        lineType = LABEL;
        char *newLine = handleLabel(line, &symbolTable);
        int secondaryLineType = checkLineType(newLine);
    }
    else if (isInstruction(parsedLine[0]))
    {
        lineType = INSTRUCTION;
    }
    else
    {
        lineType = ERROR;
    }
    return lineType;
}

char* handleString(char *line, Node **symbolTableHead) {
    int index = 0;
    char *parsedLine[2];
    parseLine(line, parsedLine);
    char *binaryLine = calloc(1, sizeof(char) * 1);

    for (int i = 0; parsedLine[1][i] != '\0'; i++) {
        if(parsedLine[1][i] == '"') { 
            continue;
        }

        int currentChar = parsedLine[1][i];
        
        char *binaryNumber = (char *)malloc(sizeof(char) * BITS_AMOUNT);
        intToBinary(currentChar, &binaryNumber);
        binaryLine = realloc(binaryLine, sizeof(char) * (strlen(binaryLine) + strlen(binaryNumber) + 1));
        if (binaryLine == NULL)
        {
            printIntError(ERROR_CODE_10);
        }
        strcat(binaryLine, binaryNumber);
        free(binaryNumber); // Free the memory allocated by intToBinary
        // add \n to the end of the line
        binaryLine = realloc(binaryLine, sizeof(char) * (strlen(binaryLine) + 2)); // +2 to accommodate for the newline and null terminator
        if (binaryLine == NULL)
        {
            printIntError(ERROR_CODE_10);
        }
        strcat(binaryLine, "\n");
    }
    printf("%s\n", binaryLine);
    return binaryLine;
}

char *handleData(char *line, Node **symbolTableHead)
{
    int wordAmount = countWords(line);
    if (wordAmount == 0)
    {
        printIntError(ERROR_CODE_31);
    }
    char *parsedLine[wordAmount];
    parseLine(line, parsedLine);
    int length = wordAmount+1; // Use wordAmount as the length of parsedLine
    char *binaryLine = calloc(1, sizeof(char) * 1); // Create a string to store the binary line

    printf("Length: %d\n", length);
    printf("Parsed line: ");
    for (int i = 0; i < length; i++)
    {
        printf("%s ", parsedLine[i]);
    }
    printf("\n");

    for (int i = 1; i < length; i++)
    {
        printf("Parsed line[%d]: %s\n", i, parsedLine[i]);
        printf("Is number: %d\n", isNumber(parsedLine[i]));
        if(isNumber(parsedLine[i]) == 1)
        {
            int number = atoi(parsedLine[i]);
            printf("%d from loop\n", number);
            char *binaryNumber = intToBinaryString(number);
            printf("%s from loop\n", binaryNumber);
            strcat(binaryLine, binaryNumber);
            printf("%s from loop\n", binaryLine);
        }
        else
        {
            /* Check if the string is a constant in the symbol table */
            int found = 0;
            Node *node = searchNodeInList(*symbolTableHead, parsedLine[i], &found);
            if (found == 1)
            {
                char *binaryNumber = intToBinaryString(node->line);
                strcat(binaryLine, binaryNumber);
            }
            else
            {
                printIntError(ERROR_CODE_33);
            }
        }
    }
    printf("%s\n", binaryLine);
    return binaryLine;
}

char *handleLabel(char *line, Node **symbolTableHead)
{
    char *parsedLine[5] = {"", "", "", "", ""}; // Create an array to store the parsed line
    parseLine(line, parsedLine);
    /*
    parsedLine[0] = <label>:
    parsedLine[1] = <instruction or declaration>
    parsedLine[2] = <operand 1>
    parsedLine[3] = <operand 2>
    parsedLine[4] = <operand 3>
	*/
    int type = checkLineType(parsedLine[1]);
    switch (type)
    {
    case INSTRUCTION:
        addNode(symbolTableHead, parsedLine[0], "code", IC + 100);
        break;
    case DATA:
    case STRING:
        addNode(symbolTableHead, parsedLine[0], "data", IC+100);
        break;
    }
    /*
    Now we need to handle the things that come after the label
    (instruction, data or string)
	The new line will be the same as the old line but without the label
	*/
	char *newLine = malloc(strlen(line) - strlen(parsedLine[0]) + 1);
    if (newLine == NULL) {
        printIntError(ERROR_CODE_10);
    }

    for (int i = strlen(parsedLine[0]) + 1; i < strlen(line); i++)
    {
        newLine[i - strlen(parsedLine[0]) - 1] = line[i];
    }
    newLine[strlen(line) - strlen(parsedLine[0]) - 1] = '\0';

	return newLine;
}

void handleConstant(char *line, Node **symbolTableHead)
{
    /* 
    Parse the line:
    parsedLine[0] = .define
    parsedLine[1] = <name>
    parsedLine[2] = "="
     parsedLine[3] = <value>
    */
    char *parsedLine[4] = {"", "", "", ""}; // Create an array to store the parsed line
    parseLine(line, parsedLine);

    /* 
    Create a new node for the symbol table:
    name = parsedLine[1]
    data = atof(parsedLine[3])
    line = mdefine (the type of the line)
    */
    addNode(symbolTableHead, parsedLine[1], "mdefine", atoi(parsedLine[3]));
}

int calcLength(char *line) {
    int length = 0;         /* Initialize the word count */
    int inWord = 0;         /* Flag to track whether currently inside a word */
    int isCommand = 0;      /* Flag to track whether a command is encountered */
    int isLabel = 0;        /* Flag to track whether a label is encountered */
    int isRegister = 0;     /* Flag to track whether a register is encountered */

    char *ptr = line;       /* Pointer to iterate through the line */

    while (*ptr != '\0') {
        if (isspace(*ptr) || *ptr == ',' || *ptr == '[' || *ptr == ']') {
            if (inWord) {
                length++;
                inWord = 0;
            }
            if (isCommand || isLabel) {
                length--;
            }
            isCommand = 0;
            isLabel = 0;
        } else {
            inWord = 1;
            if (*ptr == ':') {
                isLabel = 1;
            } else if (*ptr == 'r' && isdigit(*(ptr + 1)) && *(ptr + 2) == ',' && (isspace(*(ptr + 3)) || *(ptr + 3) == '[')) {
                isRegister = 1;
            } else if (isRegister && (isspace(*ptr) || *ptr == ',' || *ptr == '[' || *ptr == ']')) {
                isRegister = 0;
            } else if (!isdigit(*ptr) && *ptr != '[' && *ptr != ']') {
                isCommand = 1;
            }
        }
        ptr++;
    }
    if (inWord) {
        length++;
    }

    return length;
}

void executeFirstPass(char *file, char **outputFileName)
{
    FILE *inputFile = fopen(file, "r"); // Open the input file

    if (inputFile == NULL) // If the file doesn't exist
    {
        printIntError(ERROR_CODE_11); // Print an error and return
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
        int lineType = checkLineType(line); // Check the type of the line

        // make sure the constant was added to the symbol table
        // Print the cleaned line to the output file
    }
}