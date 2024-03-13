#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	char type = checkLineType(parsedLine[1]);
    switch (type)
    {
    case INSTRUCTION:
        addNode(symbolTableHead, parsedLine[0], "code", 100+IC);
		printf("IC + 100: %d\n", 100+IC);
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
	char newLine[strlen(line) - strlen(parsedLine[0])];
	for (int i = strlen(parsedLine[0]) + 1; i < strlen(line); i++)
	{
		newLine[i - strlen(parsedLine[0]) - 1] = line[i];
	}
	newLine[strlen(line) - strlen(parsedLine[0]) - 1] = '\0';
	
	Node *newNode = searchNodeInList(symbolTable, parsedLine[0], &isLabel);
	printf("Node name: %s\n", newNode->name);
	printf("Node data: %s\n", newNode->data);
	printf("Node line: %d\n", newNode->line);

	return "david";
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
    addNode(symbolTableHead, parsedLine[1], "mdefine", atof(parsedLine[3]));
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

        switch (lineType)
        {
            case COMMENT: // If the line is a comment
                break;
            case CONSTANT: // If the line is a constant, add it to the symbol table
                handleConstant(line, &symbolTable);
                break;
            case LABEL:
                isLabel = 1;
                handleLabel(line, &symbolTable);
                break;
        }

        // make sure the constant was added to the symbol table
        int found = 0;
        Node node = *searchNodeInList(symbolTable, "sz", &found);
        printf("Node name: %s\n", node.name);
        printf("Node data: %s\n", node.data);
        printf("Node line: %d\n", node.line);
        // Print the cleaned line to the output file
    }
}