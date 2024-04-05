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
int binaryLineNumber = 1; /* Initialize the isLabel flag */
int lineNumber = 1; /* Initialize the line number */
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
char *checkLineType(char *line)
{
    /* Return an empty string if the line is empty - the executer will write nothing to the output file */
    if (line[0] == ';' || line[0] == '\n')
        return "";

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
    for(int i = 0; i < wordAmount; i++)
    {
        /* Calculate the length of the string once */
        int len = strlen(parsedLine[i]);

        /* Check if the word contains a , or : */
        for(int j = 0; j < len; j++)
        {
            /* If after the : or , there is nothing - ignore it */
            if(parsedLine[i][j] == ':' || parsedLine[i][j] == ',')
            {
                if(parsedLine[i][j+1] == '\0')
                    continue;

                /* If there is something after the : or , - break it down to two words */
                char *firstWord = (char *)calloc(j + 2, sizeof(char));
                if (firstWord == NULL) {
                    printIntError(ERROR_CODE_10);
                }
                char *secondWord = (char *)calloc(len - j, sizeof(char));
                if (secondWord == NULL) {
                    printIntError(ERROR_CODE_10);
                }

                for(int k = 0; k < j+1; k++)
                {
                    firstWord[k] = parsedLine[i][k];
                }
                firstWord[j+1] = '\0';

                for(int k = j + 1; k < len; k++)
                {
                    secondWord[k - j - 1] = parsedLine[i][k];
                }

                secondWord[len - j - 1] = '\0';

                /* Reallocate the parsed line to have one more word */
                if (i == wordAmount - 1) {
                    *parsedLine = realloc(*parsedLine, sizeof(char *) * (wordAmount + 1));
                    if (parsedLine == NULL) {
                        printIntError(ERROR_CODE_10);
                    }
                }

                /* Shift all the words after the word with the : or , */
                for(int k = wordAmount - 1; k > i; k--)
                {
                    parsedLine[k + 1] = parsedLine[k];
                }

                /* Add the two new words to the parsed line */
                parsedLine[i] = firstWord;
                parsedLine[i + 1] = secondWord;

                /* Increment the word amount */
                wordAmount++;

                /* Break the loop */
                break;
            }
        }
    }

    /* Check the first word in the line is .define - constant declaration */
    if(strcmp(parsedLine[0], ".define") == 0)
    {
        /* Use handleConstant to add the constant to the symbol table */
        handleConstant(line, &symbolTable);
        /* Return nothing - the executer will write nothing to the output file */
        return "";
    }
    int isLabel = 0;

    char *newParsedLine[wordAmount-1];

    /* Check if it's a label */
    if(parsedLine[0][strlen(parsedLine[0]) - 1] == ':')
    {
        isLabel = 1;
        for(int i = 0; i < wordAmount - 1; i++)
        {
            newParsedLine[i] = parsedLine[i+1];
        }
    }
    else
    {
        for(int i = 0; i < wordAmount; i++)
        {
            newParsedLine[i] = parsedLine[i];
        }
    }

    /* Check if the first word in the line is .data - data declaration */
    if(strcmp(newParsedLine[0], ".data") == 0)
    {
        if(isLabel)
            handleLabel(parsedLine, &symbolTable, DATA);
        /* Use handleData to translate the data to binary */
        char *binaryLine;
        if(isLabel)
            binaryLine = handleData(newParsedLine, &symbolTable, wordAmount-1);
        else
            binaryLine = handleData(newParsedLine, &symbolTable, wordAmount);

        return binaryLine;
    }
    /* Check if the first word in the line is .string - string declaration */
    else if(strcmp(newParsedLine[0], ".string") == 0)
    {   
        if(isLabel)
            handleLabel(parsedLine, &symbolTable, STRING);
        /* Use handleString to translate the string to binary */
        char *binaryLine = handleString(newParsedLine[1]);
        return binaryLine;
    }
    else if(strcmp(parsedLine[0], ".entry") == 0)
    {
        return "";
    }
    else if(strcmp(parsedLine[0], ".extern") == 0)
    {
        return "";
    }
    /* If the first word ends with a colon - label declaration */
    else if(parsedLine[0][strlen(parsedLine[0]) - 1] == ':')
    {
        printf("Label\n");
        /* Create a new line without the label - translate it to binary and add the label to the symbol table */
        //char *newLine = handleLabel(line, &symbolTable);
        //printf("New Line: %s\n", newLine);
        //char *binaryLine = checkLineType(newLine);
        //printf("%s\n", binaryLine);
        /* Return the binary line - the executer will write it to the output file */
        //return binaryLine;
    }
    else if (isInstruction(parsedLine[0]))
    {
        char *binaryLine = handleInstruction(line, &symbolTable);
        return binaryLine;
    }
    else
    {
        return "ERROR";
    }
}

char *handleTwoOperands(char *operandOne, char *operandTwo, Node **symbolTableHead, char *binaryLine)
{
    int firstAddressing = 0, secondAddressing = 0;
    char *addressingMethodOne = addressingMethod(operandOne, *symbolTableHead, &firstAddressing);
    char *addressingMethodTwo = addressingMethod(operandTwo, *symbolTableHead, &secondAddressing);
    char *ARE = "00";

    char *result = (char *)calloc(15, sizeof(char));
    if (result == NULL) {
        printIntError(ERROR_CODE_10);
    }

    strcat(result, binaryLine);
    strcat(result, addressingMethodOne);
    strcat(result, addressingMethodTwo);
    strcat(result, ARE);
    strcat(result, "\n");

    free(addressingMethodOne);
    free(addressingMethodTwo);

    if(firstAddressing == secondAddressing && firstAddressing == 3)
    {
        result = (char *)realloc(result, sizeof(char) * 15);
        if (result == NULL) {
            printIntError(ERROR_CODE_10);
        }

        char *firstNumber = intToBinary(operandOne[1] - '0', 3);
        char *secondNumber = intToBinary(operandTwo[1] - '0', 3);

        strcat(result, "0000000");
        strcat(result, firstNumber);
        strcat(result, secondNumber);
        strcat(result, "0");
        strcat(result, "\n");

        return result;
    }

    if(firstAddressing == 0)
    {
        result = realloc(result, sizeof(char) * 15);
        if(result == NULL)
        {
            printIntError(ERROR_CODE_10);
        }

        int number = 0;
        int length = strlen(operandOne);
        for(int i = 1; i < length; i++)
        {
            number = number * 10 + (operandOne[i] - '0');
        }

        char *binaryNumber = intToBinary(number, 12);
        
        strcat(result, binaryNumber);
        strcat(result, "00");
        strcat(result, "\n");

        free(binaryNumber);
    }
    else if(firstAddressing == 3 && secondAddressing != 3)
    {
        /* Reallocate the result to have 14 more bits */
        result = realloc(result, sizeof(char) * 15);
        if (result == NULL) {
            printIntError(ERROR_CODE_10);
        }

        char *binaryNumber = intToBinary(operandOne[1] - '0', 3);

        strcat(result, "0000000");
        strcat(result, binaryNumber);
        strcat(result, "0000\n");
    }

    /* Operate the same like we did on the first operand but on the second operand */
    if(secondAddressing == 0)
    {
        result = realloc(result, sizeof(char) * 15);
        if(result == NULL)
        {
            printIntError(ERROR_CODE_10);
        }

        int number = 0;
        int length = strlen(operandTwo);
        for(int i = 1; i < length; i++)
        {
            number = number * 10 + (operandTwo[i] - '0');
        }

        char *binaryNumber = intToBinary(number, 12);
        
        strcat(result, binaryNumber);
        strcat(result, "00\n");
    }
    else if(secondAddressing == 3 && firstAddressing != 3)
    {
        /* Reallocate the result to have 14 more bits */
        result = realloc(result, sizeof(char) * 15);
        if (result == NULL) {
            printIntError(ERROR_CODE_10);
        }

        char *binaryNumber = intToBinary(operandTwo[1] - '0', 3);

        strcat(result, "0000000000");
        strcat(result, binaryNumber);
        strcat(result, "0\n");
    }

    return result;    
}

char *handleOneOperand(char *operand, Node **symbolTableHead, char *binaryLine)
{
    int addressing = 0;
    char *methodAddressing = addressingMethod(operand, *symbolTableHead, &addressing);
    char *ARE = "00";

    char *result = (char *)calloc(30, sizeof(char));
    if (result == NULL) {
        printIntError(ERROR_CODE_10);
    }

    /*
    bits 0 - 1: ARE
    bits 2 - 3: 00 (not used)
    bits 4 - 5: addressing method
    bits 6 - 9: opcode
    bits 10 - 13: 0000 (not used)
    bit 14: '\n'
    */
    strcat(result, binaryLine);
    strcat(result, methodAddressing);
    strcat(result, "00");
    strcat(result, ARE);
    strcat(result, "\n");

    if(addressing == 0)
    {
        result = realloc(result, sizeof(char) * 30);
        if(result == NULL)
        {
            printIntError(ERROR_CODE_10);
        }

        int number = 0;
        int length = strlen(operand);
        for(int i = 1; i < length; i++)
        {
            number = number * 10 + (operand[i] - '0');
        }

        char *binaryNumber = intToBinary(number, 12);
        
        strcat(result, binaryNumber);
        strcat(result, "00\n");

        free(binaryNumber);
    }
    else if(addressing == 3)
    {
        result = realloc(result, sizeof(char) * 30);
        if(result == NULL)
        {
            printIntError(ERROR_CODE_10);
        }

        char *binaryNumber = intToBinary(operand[1] - '0', 3);
        
        strcat(result, "0000000");
        strcat(result, binaryNumber);
        strcat(result, "0000\n");
    }

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

        char *binaryLine = (char *)calloc(9, sizeof(char));
        if (binaryLine == NULL) {
            printIntError(ERROR_CODE_10);
        }

        strcat(binaryLine, "0000");
        strcat(binaryLine, instructionsInBinary[instructionIndex]);

        char *result = handleTwoOperands(parsedLine[1], parsedLine[2], symbolTableHead, binaryLine);

        return result;
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

        char *binaryLine = (char *)calloc(9, sizeof(char));
        if (binaryLine == NULL) {
            printIntError(ERROR_CODE_10);
        }

        strcat(binaryLine, "0000");
        strcat(binaryLine, instructionsInBinary[instructionIndex]);

        char *result = handleOneOperand(parsedLine[1], symbolTableHead, binaryLine);

        return result;
    }

    if(instructionIndex == 14 || instructionIndex == 15)
    {
        if(wordAmount > 1)
        {
            printIntError(ERROR_CODE_31);
        }

        if(instructionIndex == 14)
        {
            char *result = (char *)calloc(15, sizeof(char));
            if (result == NULL) {
                printIntError(ERROR_CODE_10);
            }

            strcat(result, "00001110000000\n");

            return result;
        }
        else
        {
            char *result = (char *)calloc(15, sizeof(char));
            if (result == NULL) {
                printIntError(ERROR_CODE_10);
            }

            strcat(result, "00001111000000\n");

            return result;
        }
    }
}

/* Purpose is explained in the header file */
char* handleString(char *line) {
    int amountOfChars = 0;
    for(int i = 0; line[i] != '\0'; i++)
    {
        if(line[i] == '"')
        {
            continue;
        }
        amountOfChars++;
    }

    int lengthOfBinaryLine = (amountOfChars+1) * 15; /* Calculate the length of the binary line */

    char *binaryLine = (char *)calloc(lengthOfBinaryLine, sizeof(char)); /* Create a new string to store the binary line */

    /* Loop through the string */
    int i = 0;
    for (i = 0; i < amountOfChars; i++){
        /* If the character is a quotation mark (start or end of the string) - skip it */
        if(line[i] == '"')
            continue;
        /* Convert the character to ascii and then to binary */
        int number = line[i];
        /* Convert the number to a binary string */
        char *binaryNumber = intToBinary(number, BITS_AMOUNT);
        /* Add the binary number to the binary line */
        if(binaryLineNumber == 1)
            strcat(binaryLine, binaryNumber);
        else {
            strcat(binaryLine, "\n");
            strcat(binaryLine, binaryNumber);
        }

        binaryLineNumber++;
        free(binaryNumber);
    }
    /* Code a null terminator to the binary line */
    char *binaryNumber = "00000000000000";
    strcat(binaryLine, "\n");
    strcat(binaryLine, binaryNumber);
    binaryLineNumber++;
    free(binaryNumber);

    /* Return the binary line */
    return binaryLine;
}

/* Purpose is explained in the header file */
char *handleData(char *parsedLine[], Node **symbolTableHead, int wordAmount)
{
    char *binaryLine = (char *)calloc(wordAmount * 15, sizeof(char)); /* Create a new string to store the binary line */
    if(binaryLine == NULL)
    {
        printIntError(ERROR_CODE_10);
    }
    /* Loop through the parsed line */

    for (int i = 1; i < wordAmount; i++)
    {
        /* Check if the string is a number */
        if(isNumber(parsedLine[i]) == 1)
        {
            /* Convert the string to an integer */
            int number = atoi(parsedLine[i]);
            /* Convert the number to a binary string */
            char *binaryNumber = intToBinary(number, BITS_AMOUNT);
            /* Add the binary number to the binary line */
            if(binaryLineNumber == 1)
                strcat(binaryLine, binaryNumber);
            else {
                strcat(binaryLine, "\n");
                strcat(binaryLine, binaryNumber);
            }
            binaryLineNumber++;
            free(binaryNumber);
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
                char *binaryNumber = intToBinary(node->line, BITS_AMOUNT);
                /* Add the binary number to the binary line */
                if(binaryLineNumber == 1)
                    strcat(binaryLine, binaryNumber);
                else {
                    strcat(binaryLine, "\n");
                    strcat(binaryLine, binaryNumber);
                }
                binaryLineNumber++;
                free(binaryNumber);
            }
            else
                printIntError(ERROR_CODE_33); /* Print an error */
        }
    }
    /* Add a null terminator to the binary line */
    //binaryLine[lengthOfBinaryLine] = '\0';
    /* Return the binary line */
    return binaryLine;
}

/* Purpose is explained in the header file */
void *handleLabel(char *parsedLine[], Node **symbolTableHead, int type)
{
    /*
    parsedLine[0] = <label>:
    parsedLine[1] = <instruction or declaration>
    parsedLine[2] = <operand 1>
    ...
	*/
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
            addNode(symbolTableHead, parsedLine[0], "data", DC);
            break;
    }
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

    /* Check if the constant is a number */
    if (isNumber(parsedLine[3]) == 0)
    {
        printIntError(ERROR_CODE_31); /* Print an error and return */
        return;
    }
    
    /* Make sure the constant is not already in the symbol table */
    int found = 0;
    searchNodeInList(*symbolTableHead, parsedLine[1], &found);
    if (found == 1)
        printIntError(ERROR_CODE_33);
    /* Add the constant to the symbol table */
    else
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
        printIntError(ERROR_CODE_11); // Print an error and return

    char outputName[MAX_LINE_LENGTH] = "";
    strcpy(outputName, file);

    int dotIndex = 0;
    while (outputName[dotIndex++] != '.');

    outputName[dotIndex++] = 'o';
    outputName[dotIndex++] = 'b';
    outputName[dotIndex++] = 'j';
    outputName[dotIndex] = '\0';

    *outputFileName = (char *)calloc(strlen(outputName) + 1, sizeof(char));
    if (*outputFileName == NULL)
        printIntError(ERROR_CODE_10);
    strcpy(*outputFileName, outputName);

    FILE *outputFile = fopen(outputName, "w");
    if (outputFile == NULL)
        printIntError(ERROR_CODE_13);

    char line[MAX_LINE_LENGTH] = ""; // Create a buffer to store the line

    while(fgets(line, MAX_LINE_LENGTH, inputFile) != NULL) // Loop through the input file
    {
        char *cleanedLine = removeCommas(cleanLine(line)); // Clean the line
        char *binaryLine = checkLineType(cleanedLine); // Check the type of the line

        // make sure the constant was added to the symbol table
        // Print the cleaned line to the output file

        if (strcmp(binaryLine, "ERROR") != 0)
        {
            fprintf(outputFile, "%s", binaryLine);
        }
        free(cleanedLine); // Free the cleaned line
        free(binaryLine); // Free the binary line
    }

    fclose(inputFile); // Close the input file
    fclose(outputFile); // Close the output file

    /* Update every symbol that is a data type by adding IC + 100 */
    /* Run through the symbol table */
    Node *current = symbolTable;
    while (current != NULL)
    {
        /* If the symbol is a data type */
        if (strcmp(current->data, "data") == 0)
        {
            /* Update the line */
            current->line += IC + 100;
        }
        /* Move to the next node */
        current = current->next;
    }
}