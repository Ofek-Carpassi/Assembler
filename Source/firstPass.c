#include <stdio.h> /* used for file operations */
#include <stdlib.h> /* used for malloc */
#include <string.h>  /* used for strlen */
#include <ctype.h> /* used for isdigit */
#include "../Headers/firstPass.h" /* Include the header file with the first pass functions */
#include "../Headers/errors.h" /* Include the header file with the error codes */
#include "../Headers/globalVariables.h" /* Include the header file with the global variables */
#include "../Headers/utilities.h" /* Include the header file with the utility functions */
#include "../Headers/dataStructers.h" /* Include the header file with the data structures */

int IC = 0, DC = 0; /* Initialize the instruction counter and the data counter */
int isLabel = 0; /* Initialize the isLabel flag */
const char *instructionNames[] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "hlt"}; /* Array of the instruction names */
const char *instructionsInBinary[] = {"0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111", "1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111"}; /* Array of the instruction names in binary */
Node *symbolTable = NULL; /* Create the symbol table */

/* Purpose is explained in the header file */
int isInstruction(char *word)
{
    /* Loop through the instruction names */
    for (int i = 0; i < OPCODES_COUNT; i++)
    {
        /* Check if the word got from the call is an instruction */
        if (strcmp(word, instructionNames[i]) == 0)
        {
            /* If it is, return 1 */
            return 1;
        }
    }
    /* If it's not, return 0 */
    return 0;
}

/* Purpose is explained in the header file */
int checkLineType(char *line)
{
    /* Initialize the line type */
    int lineType = 0;

    /* If the line is a comment, set the line type to comment */
    if (line[0] == ';' || line[0] == '\n') // If the line is a comment
    {
        return COMMENT; /* Return the comment type - the program will ignore this line */
    }

    int wordAmount = countWords(line); /* Count the words in the line */
    if (wordAmount == 0) /* If the are no words in the line */
    {
        printIntError(ERROR_CODE_31); /* Print an error and return */
        return ERROR;
    }
    /* Create an array to store the parsed line */
    char *parsedLine[wordAmount];
    /* Parse the line */
    parseLine(line, parsedLine);

    /* Check the first word in the line is .define - constant declaration */
    if(strcmp(parsedLine[0], ".define") == 0)
    {
        /* Use handleConstant to add the constant to the symbol table */
        handleConstant(line, &symbolTable);
        return CONSTANT;
    }
    /* Check if the first word in the line is .data - data declaration */
    else if(strcmp(parsedLine[0], ".data") == 0)
    {
        /* Use handleData to translate the data to binary */
        char *binaryLine = handleData(line, &symbolTable);
        printf("%s\n", binaryLine);
        return DATA;
    }
    /* Check if the first word in the line is .string - string declaration */
    else if(strcmp(parsedLine[0], ".string") == 0)
    {
        /* Use handleString to translate the string to binary */
        char *binaryLine = handleString(line);
        printf("%s\n", binaryLine);
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
    /* If the first word ends with a colon - label declaration */
    else if(parsedLine[0][strlen(parsedLine[0]) - 1] == ':')
    {
        /* Create a new line without the label - translate it to binary and add the label to the symbol table */
        char *newLine = handleLabel(line, &symbolTable);
        int secondaryLineType = checkLineType(newLine);
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

char *handleTwoOperands(char *operandOne, char *operandTwo, Node **symbolTableHead, char *binaryLine)
{
    char *addressingMethodOne = addressingMethod(operandOne, *symbolTableHead);
    char *addressingMethodTwo = addressingMethod(operandTwo, *symbolTableHead);
    char *ARE = "00";

    /* Binary line is built as follows:
    0 - 1 bits - ARE
    2 - 3 bits - addressing method of the first operand
    4 - 5 bits - addressing method of the second operand
    6 - 9 bits - opcode
    10 - 13 bits - unused
    We'll need to add bits 0 - 5 to the binary line
    */
    char *result = calloc(1, sizeof(char) * 14);
    if (result == NULL) {
        printIntError(ERROR_CODE_10);
    }
    
    
    /* Add the binary line we got from the instruction to the result */
    strcat(result, binaryLine);
    /* Add the addressing method of the first operand to the result */
    strcat(result, addressingMethodOne);
    /* Add the addressing method of the second operand to the result */
    strcat(result, addressingMethodTwo);
    /* Add the ARE bits to the result */
    strcat(result, ARE);

    /* Reverse the result */

    return result;    
}

/* Purpose is explained in the header file */
char *handleInstruction(char *line, Node **symbolTableHead)
{
    int wordAmount = countWords(line); /* Count the words in the line */
    if (wordAmount == 0) /* If the are no words in the line */
    {
        printIntError(ERROR_CODE_31); /* Print an error and return */
    }
    char *parsedLine[wordAmount]; /* Create an array to store the parsed line */
    parseLine(line, parsedLine); /* Parse the line */

    /* Find the instruction in the instruction names array */
    int instructionIndex = 0;
    for (int i = 0; i < OPCODES_COUNT; i++)
    {
        if (strcmp(parsedLine[0], instructionNames[i]) == 0)
        {
            instructionIndex = i;
            break;
        }
    }

    if((instructionIndex >= 0 && instructionIndex <= 3) || instructionIndex == 6)
    {
        if(wordAmount > 3)
        {
            printIntError(ERROR_CODE_31);
        }
        else if(wordAmount < 3)
        {
            printIntError(ERROR_CODE_32);
        }
    }

    if(instructionIndex == 4 || instructionIndex == 5 || (instructionIndex >= 7 && instructionIndex <= 13))
    {
        if(wordAmount > 2)
        {
            printIntError(ERROR_CODE_31);
        }
        else if(wordAmount < 2)
        {
            printIntError(ERROR_CODE_32);
        }
    }

    if(instructionIndex == 14 || instructionIndex == 15)
    {
        if(wordAmount > 1)
        {
            printIntError(ERROR_CODE_31);
        }
    }
}

/* Purpose is explained in the header file */
char* handleString(char *line) {
    /* Create an array to store the parsed line */
    int wordAmount = countWords(line); /* Count the words in the line */
    if (wordAmount == 0) /* If the are no words in the line */
    {
        printIntError(ERROR_CODE_32); /* Print an error and return */
    }
    else if(wordAmount > 2) /* If there are more than 2 words in the line */
    {
        printIntError(ERROR_CODE_31); /* Print an error and return */
    }
    char *parsedLine[wordAmount]; /* Create an array to store the parsed line */
    parseLine(line, parsedLine); /* Parse the line */

    /* Create a new string to store the binary line */
    char *binaryLine = calloc(1, sizeof(char) * 1); 
    if (binaryLine == NULL) {
        printIntError(ERROR_CODE_10);
    }

    /* Loop through the string */
    int i = 0;
    for (i = 0; parsedLine[1][i] != '\0'; i++) {
        /* If the character is a quotation mark (start or end of the string) - skip it */
        if(parsedLine[1][i] == '"') { 
            continue;
        }
        /* Convert the character to ascii and then to binary */
        int number = parsedLine[1][i];
        /* Convert the number to a binary string */
        char *binaryNumber = intToBinaryString(number);
        /* Add the binary number to the binary line */
        strcat(binaryLine, binaryNumber);
    }
    /* Add a null terminator to the binary line */
    int length = strlen(binaryLine);
    binaryLine[length] = '\0';
    /* Return the binary line */
    return binaryLine;
}

/* Purpose is explained in the header file */
char *handleData(char *line, Node **symbolTableHead)
{
    /* Create an array to store the parsed line */
    int wordAmount = countWords(line); /* Count the words in the line */
    if (wordAmount == 0)
    {
        printIntError(ERROR_CODE_31);
    }
    char *parsedLine[wordAmount]; /* Create an array to store the parsed line */
    parseLine(line, parsedLine); /* Parse the line */

    int length = wordAmount; /* Calculate the length of the binary line */

    char *binaryLine = calloc(1, sizeof(char) * 1); /* Create a new string to store the binary line */
    if (binaryLine == NULL) {
        printIntError(ERROR_CODE_10);
    }

    /* Loop through the parsed line */
    for (int i = 1; i < length; i++)
    {
        /* Check if the string is a number */
        if(isNumber(parsedLine[i]) == 1)
        {
            /* Convert the string to an integer */
            int number = atoi(parsedLine[i]);
            /* Convert the number to a binary string */
            char *binaryNumber = intToBinaryString(number);
            /* Add the binary number to the binary line */
            strcat(binaryLine, binaryNumber);
        }
        else
        {
            /* Check if the string is a constant in the symbol table */
            int found = 0;
            Node *node = searchNodeInList(*symbolTableHead, parsedLine[i], &found);
            /* If the constant is found in the symbol table */
            if (found == 1)
            {
                /* Convert the constant to a binary string */
                char *binaryNumber = intToBinaryString(node->line);
                /* Add the binary number to the binary line */
                strcat(binaryLine, binaryNumber);
            }
            else
                printIntError(ERROR_CODE_33); /* Print an error */
        }
    }
    /* Add a null terminator to the binary line */
    length = strlen(binaryLine);
    binaryLine[length] = '\0';
    /* Return the binary line */
    return binaryLine;
}

/* Purpose is explained in the header file */
char *handleLabel(char *line, Node **symbolTableHead)
{
    /* Create an array to store the parsed line */
    int wordAmount = countWords(line); /* Count the words in the line */
    if (wordAmount == 0)
    {
        printIntError(ERROR_CODE_31);
    }
    char *parsedLine[wordAmount]; /* Create an array to store the parsed line */
    parseLine(line, parsedLine); /* Parse the line */
    /*
    parsedLine[0] = <label>:
    parsedLine[1] = <instruction or declaration>
    parsedLine[2] = <operand 1>
    ...
	*/
    /* Check of the type of the second word in the line (code or data) */
    int type = checkLineType(parsedLine[1]);
    switch (type)
    {
        /* If the type is an instruction */
        case INSTRUCTION:
            /* Add the label to the symbol table with the type code */
            addNode(symbolTableHead, parsedLine[0], "code", IC + 100);
            break;
        /* If the type is a data or string declaration */
        case DATA:
        case STRING:
            /* Add the label to the symbol table with the type data */
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

    /* Copy the line to the new line without the label */
    for (int i = strlen(parsedLine[0]) + 1; i < strlen(line); i++)
    {
        newLine[i - strlen(parsedLine[0]) - 1] = line[i];
    }
    newLine[strlen(line) - strlen(parsedLine[0]) - 1] = '\0';

    /* Return the new line */
	return newLine;
}

/* Purpose is explained in the header file */
void handleConstant(char *line, Node **symbolTableHead)
{
    /* Create an array to store the parsed line */
    int wordAmount = countWords(line); /* Count the words in the line */
    if (wordAmount == 0)
    {
        printIntError(ERROR_CODE_31);
    }
    char *parsedLine[wordAmount]; /* Create an array to store the parsed line */
    parseLine(line, parsedLine); /* Parse the line */

    /* 
    Create a new node for the symbol table:
    name = parsedLine[1]
    data = atof(parsedLine[3])
    line = mdefine (the type of the line)
    */
    addNode(symbolTableHead, parsedLine[1], "mdefine", atoi(parsedLine[3]));
}

/* Only a prototype - still not working */
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