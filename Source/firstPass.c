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
int lineNumber = 1; /* Initialize the line number */
int lineNumberSrcFile = 1; /* Initialize the line number of the source file */
Node *symbolTable = NULL; /* Create the symbol table */
char *fileName = NULL; /* Create the file name */

/* The names of the instructions */
const char *instructionNames[] = {
    "mov",
    "cmp",
    "add",
    "sub",
    "not",
    "clr",
    "lea",
    "inc",
    "dec",
    "jmp",
    "bne",
    "red",
    "prn",
    "jsr",
    "rts",
    "hlt"
};

/* The amount of instructions */
#define OPCODES_COUNT 16

typedef struct OPCODE {
    char *binary;
    int arguments;
} Opcode;

const Opcode opcodes[] = {
    {"0000", 2}, // mov
    {"0001", 2}, // cmp
    {"0010", 2}, // add
    {"0011", 2}, // sub
    {"0100", 1}, // not
    {"0101", 1}, // clr
    {"0110", 2}, // lea
    {"0111", 1}, // inc
    {"1000", 1}, // dec
    {"1001", 1}, // jmp
    {"1010", 1}, // bne
    {"1011", 1}, // red
    {"1100", 1}, // prn
    {"1101", 1}, // jsr
    {"1110", 0}, // rts
    {"1111", 0}  // hlt
};

/* Purpose is explained in the header file */
int isInstruction(char *word)
{
    /* Loop through the instruction names */
    for (int i = 0; i < OPCODES_COUNT; i++)
    {
        /* Check if the word got from the call is an instruction */
        if (strcmp(word, instructionNames[i]) == 0)
        {
            /* return the index of the instruction */
            return i;
        }
    }
    /* Return -1 if the word is not an instruction */
    return -1;
}

/* Purpose is explained in the header file */
char *checkLineType(char *line)
{
    /* Return an empty string if the line is empty - the executer will write nothing to the output file */
    if (line[0] == ';' || line[0] == '\n')
        return "";

    int wordAmount = countWords(line); /* Count the words in the line */
    int originalWordAmount = wordAmount;
    if (wordAmount == 0) /* If the are no words in the line */
        printIntError(ERROR_CODE_45); /* Print an error */
    /* Allocate memory for the parsed line */
    char **parsedLine = parseLine(line, wordAmount);

    /* print the parsed line */
    for(int i = 0; i < wordAmount; i++)
        printf("Parsed line[%d]: %s\n", i, parsedLine[i]);

    /* run on the first word and check if there's a : somewhere in the word */
    for(int i = 0; i < strlen(parsedLine[0])-1; i++)
    {
        if(parsedLine[0][i] == ':')
        {
            wordAmount++;
            /* save everything until the : including it in a string */
            char *label = (char *)calloc(i+1, sizeof(char));
            if (label == NULL)
                printIntError(ERROR_CODE_10);
            for(int j = 0; j < i+1; j++)
                label[j] = parsedLine[0][j];
            /* save everything after the : in a string */
            char *restOfLine = (char *)calloc(strlen(parsedLine[0]) - i, sizeof(char));
            if (restOfLine == NULL)
                printIntError(ERROR_CODE_10);
            for(int j = i+1; j < strlen(parsedLine[0]); j++)
                restOfLine[j-i-1] = parsedLine[0][j];
            /* Reallocate the parsed line to have one more word */
            parsedLine = (char **)realloc(parsedLine, sizeof(char *) * wordAmount);
            if (parsedLine == NULL)
                printIntError(ERROR_CODE_10);
            /* move all the words one index to the right, remove the first word
            put the label in the first index and the rest of the line in the second index.
            all the rest of the words will be moved one index to the right */
            for(int j = wordAmount-1; j > 1; j--)
            {
                parsedLine[j] = parsedLine[j-1];
            }
            parsedLine[0] = label;
            parsedLine[1] = restOfLine;
            break;
        }
    }

    /* Check the first word in the line is .define - constant declaration */
    if(strcmp(parsedLine[0], ".define") == 0)
    {
        handleConstant(parsedLine, &symbolTable, wordAmount);
        /* Return nothing - the executer will write nothing to the output file */
        free(parsedLine);
        return "CONSTANT";
    }
    int isLabel = 0;

    /* Check if it's a label */
    int len = strlen(parsedLine[0]) - 1;
    if(parsedLine[0][len] == ':')
        isLabel = 1;

    /* We won't use this variable if it's not a label - it doesn't matter what it contains if it isn't a label */
    char *label = (char *)calloc(strlen(parsedLine[0]), sizeof(char));
    /* Remove the : from the label */
    strcpy(label, parsedLine[0]);
    label[len] = '\0';
    if(isLabel)
    {
        
        /* int i = last word index */
        printf("Word amount: %d\n", wordAmount);
        for(int i = 0; i < wordAmount; i++){
            printf("i: %d, Parsed line[%d]: %s\n", i, i, parsedLine[i]);
            parsedLine[i] = parsedLine[i+1];
        }

        wordAmount--;
    }

    printf("reparsed the line\n");
    for(int i = 0; i < wordAmount; i++)
        printf("Parsed line[%d]: %s\n", i, parsedLine[i]);

    /* Check if the first word in the line is .data - data declaration */
    if(strcmp(parsedLine[0], ".data") == 0)
    {
        if(isLabel)
            handleLabel(label, &symbolTable, DATA);
            
        /* Use handleData to translate the data to binary */
        char *binaryLine = handleData(parsedLine, &symbolTable, wordAmount);

        DC += wordAmount - 1; /* Increment the data counter by the number of numbers in the data declaration */
        calcLength(parsedLine, wordAmount);
        if(isLabel)
            IC++;
        free(parsedLine);
        return binaryLine;
    }
    /* Check if the first word in the line is .string - string declaration */
    else if(strcmp(parsedLine[0], ".string") == 0)
    {   
        if(isLabel)
            handleLabel(label, &symbolTable, STRING);
        /* Use handleString to translate the string to binary */
        char *binaryLine = handleString(parsedLine[1]);
        calcLength(parsedLine, wordAmount);
        if(isLabel)
            IC++;
        free(parsedLine);
        return binaryLine;
    }
    /* Check if the first word in the line is .extern */
    else if(strcmp(parsedLine[0], ".extern") == 0)
    {
        for(int i = 1; i < wordAmount; i++)
            addNode(&symbolTable, parsedLine[i], "external", 0);
        calcLength(parsedLine, wordAmount);
        free(parsedLine);
        return "";
    }
    printf("Is label: %d\n", isLabel);
    if(isLabel)
        handleLabel(label, &symbolTable, INSTRUCTION);
    printf("Parsed line[0]: %s\n", parsedLine[0]);
    if(parsedLine[0][strlen(parsedLine[0])-1] == '\n')
        parsedLine[0][strlen(parsedLine[0])-1] = '\0';
    int instructionIndex = isInstruction(parsedLine[0]);
    printf("Instruction index: %d\n", instructionIndex);
    if(instructionIndex != -1)
    {
        printf("Calling handle instruction\n");
        /* Use handleInstruction to translate the instruction to binary */
        char *binaryLine = handleInstruction(parsedLine, &symbolTable, wordAmount);
        printf("Binary line: %s\n", binaryLine);
        calcLength(parsedLine, wordAmount);
        printf("new IC: %d\n", IC);

        free(parsedLine);

        return binaryLine;
    }
}

char *operandHandling(char *operand, Node **symbolTableHead, int addressingMethod, int isConstant, int isSource)
{
    /* 
    This function doesn't handle the case of two registers
    This function knows how to handle the case of only registers and only numbers
    If the operand is a number, we translate it to 12 bits binary and add 00 at the end
    If the operand is a constant, we check if it's in the symbol table and translate it to 12 bits binary and add 00 at the end
    If the operand's a direct addressing it means it's a label, we check if it's in the symbol table and translate it to 12 bits binary and add 10 at the end
    If it's a register and its a destination operand the register number will be coded at bits 2-4.
    If it's a register and its a source operand the register number will be coded at bits 5-7.
    */
    if(operand[0] == '#' && addressingMethod == 0)
    {
        printList(*symbolTableHead);
        /* Remove the # from the operand */
        char *variable = (char *)calloc(strlen(operand), sizeof(char));
        for(int i = 1; i < strlen(operand); i++)
            variable[i-1] = operand[i];

        if(variable[strlen(variable)-1] == '\n')
            variable[strlen(variable)-1] = '\0';

        printf("Variable: %s\n", variable);
        /* Check if the string is a number or a constant */
        if(isNumber(variable) == 1 && !isConstant)
        {
            /* Convert the string to an integer */
            int number = atoi(variable);
            /* Convert the number to a binary string */
            char *binaryNumber = intToBinary(number, 12);
            /* Add the binary number to the binary line */
            char *result = (char *)calloc(15, sizeof(char));
            if (result == NULL)
                printIntError(ERROR_CODE_10);
            strcat(result, binaryNumber);
            strcat(result, "00\n");
            free(binaryNumber);
            free(variable);
            return result;
        }
        else if (isConstant)
        {
            /* Check if the index is a constant in the symbol table */
            int found = 0;
            Node *node = searchNodeInList(*symbolTableHead, variable, &found);
            printf("Found: %d\n", found);
            /* If the constant is found in the symbol table */
            if (found == 1)
            {
                /* Convert the constant to a binary string */
                char *binaryNumber = intToBinary(node->line, 3);
                /* Add the binary number to the binary line */
                char *result = (char *)calloc(15, sizeof(char));
                if (result == NULL)
                    printIntError(ERROR_CODE_10);
                strcat(result, "0000000");
                strcat(result, binaryNumber);
                strcat(result, "0000\n");
                free(binaryNumber);
                return result;
            }
            else
                printExtError(ERROR_CODE_46, (location){fileName, lineNumberSrcFile}); /* Print an error */
        }
    }
    else if(addressingMethod == 1)
    {
        /* Search for the label in the symbol table - could be with the type "data" or "external"
        If the label is found in the symbol table with the type "data" - convert the label to a binary string and add 10 at the end
        If the label is found in the symbol table with the type "external" - convert the label to a binary string and add 01 at the end
        If the label is not found in the symbol table - second pass will handle it - return an empty string */
        int found = 0;
        Node *node = searchNodeInList(*symbolTableHead, operand, &found);
        if (found == 1 && strcmp(node->data, "data") == 0)
        {
            /* Convert the label to a binary string */
            char *binaryNumber = intToBinary(node->line, 12);
            /* Add the binary number to the binary line */
            char *result = (char *)calloc(15, sizeof(char));
            if (result == NULL)
                printIntError(ERROR_CODE_10);
            strcat(result, binaryNumber);
            strcat(result, "10\n");
            free(binaryNumber);
            return result;
        }
        else if (found == 1 && strcmp(node->data, "external") == 0)
        {
            /* Convert the label to a binary string */
            char *binaryNumber = intToBinary(node->line, 12);
            /* Add the binary number to the binary line */
            char *result = (char *)calloc(15, sizeof(char));
            if (result == NULL)
                printIntError(ERROR_CODE_10);
            strcat(result, binaryNumber);
            strcat(result, "01\n");
            free(binaryNumber);
            return result;
        }
        else
            return "\n";
    }
    /* registers */
    else if (operand[0] == 'r' && addressingMethod == 3)
    {
        /* Get the register number */
        int registerNumber = operand[1] - '0';
        /* Convert the register number to a binary string */
        if(registerNumber < 0 || registerNumber > 7)
            printExtError(ERROR_CODE_32, (location){fileName, lineNumberSrcFile});
        char *binaryNumber = intToBinary(registerNumber, 3);
        /* Add the binary number to the binary line */
        char *result = (char *)calloc(15, sizeof(char));
        if (result == NULL)
            printIntError(ERROR_CODE_10);
        if(isSource)
        {
            strcat(result, "0000000");
            strcat(result, binaryNumber);
            strcat(result, "0000\n");
        }
        else
        {
            strcat(result, "000000000");
            strcat(result, binaryNumber);
            strcat(result, "00\n");
        }
        free(binaryNumber);
        return result;
    }
    else if(addressingMethod == -1)
    {
        /* Check if the index is a number or a constant */
        printf("Operand: %s\n", operand);
        printf("Is constant: %d\n", isConstant);
        if(isNumber(operand) == 1 && !isConstant)
        {
            /* Convert the index to an integer */
            int index = atoi(operand);
            /* Convert the index to a binary string */
            char *binaryNumber = intToBinary(index, 3);
            /* Add the binary number to the binary line */
            char *result = (char *)calloc(15, sizeof(char));
            if (result == NULL)
                printIntError(ERROR_CODE_10);
            strcat(result, "0000000");
            strcat(result, binaryNumber);
            strcat(result, "0000\n");
            free(binaryNumber);
            return result;
        }
        else if (isConstant)
        {
            /* Check if the index is a constant in the symbol table */
            int found = 0;
            Node *node = searchNodeInList(*symbolTableHead, operand, &found);
            /* If the constant is found in the symbol table */
            if (found == 1)
            {
                /* Convert the constant to a binary string */
                char *binaryNumber = intToBinary(node->line, 3);
                /* Add the binary number to the binary line */
                char *result = (char *)calloc(15, sizeof(char));
                if (result == NULL)
                    printIntError(ERROR_CODE_10);
                strcat(result, "0000000");
                strcat(result, binaryNumber);
                strcat(result, "0000\n");
                free(binaryNumber);
                return result;
            }
            else
                printExtError(ERROR_CODE_46, (location){fileName, lineNumberSrcFile}); /* Print an error */
        }
    }
    else if(addressingMethod == 2)
    {
        /* Check if the label is in the symbol table with the type "data" */
        int found = 0;
        Node *node = searchNodeInList(*symbolTableHead, operand, &found);
        /* If the label is found in the symbol table */
        if (found == 1)
        {
            /* Convert the label to a binary string */
            char *binaryNumber = intToBinary(node->line, 12);
            /* Add the binary number to the binary line */
            char *result = (char *)calloc(15, sizeof(char));
            if (result == NULL)
                printIntError(ERROR_CODE_10);
            strcat(result, binaryNumber);
            strcat(result, "10\n");
            free(binaryNumber);
            return result;
        }
        else
            return "\n";
    }

    return NULL;
}

char *handleTwoRegisters(char *soruceRegister, char *destinationRegister)
{
    /*
    The number of the source register will be coded at bits 5-7.
    The number of the destination register will be coded at bits 2-4.
    */
    int sourceRegisterNumber = soruceRegister[1] - '0';
    int destinationRegisterNumber = destinationRegister[1] - '0';

    if(sourceRegisterNumber < 0 || sourceRegisterNumber > 7)
        printExtError(ERROR_CODE_32, (location){fileName, lineNumberSrcFile});

    if(destinationRegisterNumber < 0 || destinationRegisterNumber > 7)
        printExtError(ERROR_CODE_32, (location){fileName, lineNumberSrcFile});

    char *sourceBinary = intToBinary(sourceRegisterNumber, 3);
    char *destinationBinary = intToBinary(destinationRegisterNumber, 3);
    char *result = (char *)calloc(15, sizeof(char));
    if (result == NULL)
        printIntError(ERROR_CODE_10);
        
    strcat(result, "000000");
    strcat(result, sourceBinary);
    strcat(result, destinationBinary);
    strcat(result, "00\n");

    free(sourceBinary);
    free(destinationBinary);
    return result;
}

/* Get the parsed line array, the symbol table and the index of the instruction 
the parsedLine array is defined as char **parsedLine = (char **)calloc(wordAmount, sizeof(char *)); */
char *handleInstruction(char **parsedLine, Node **symbolTableHead, int wordAmount)
{
    printf("Handle instruction is called\n");
    printf("parsedLine[0]: %s\n", parsedLine[0]);
    /* First we'll get the instruction */
    int instructionIndex = isInstruction(parsedLine[0]);
    /* Get the opcode */
    char *opcode = opcodes[instructionIndex].binary;
    /* Get the number of arguments */
    int arguments = opcodes[instructionIndex].arguments;
    /* Now there are three cases:
    1. The instruction's operands are two registers
    2. At least one of the operands addressing method is 10 - index addressing method
    3. None of the above
    If the first case - call handleTwoRegisters with the two operands
    If the second case - call operandHandling 3-4 (depends on the amount of indexes) times
    If the third case - call operandHandling for each operand

    Get the addressing method for each operand
    If the addressing method is 00 - immediate addressing method
    If the addressing method is 01 - direct addressing method
    If the addressing method is 10 - index addressing method
    If the addressing method is 11 - register addressing method
    */
    if(arguments == 0)
    {
        /* make sure parsed line has only the instruction - no operands */
        if(wordAmount != 1)
            printExtError(ERROR_CODE_47, (location){fileName, lineNumberSrcFile});

        char *result = (char *)calloc(16, sizeof(char));
        if (result == NULL)
            printIntError(ERROR_CODE_10);
        if(lineNumber == 1)
            strcat(result, "0000");
        else
            strcat(result, "\n0000");
        strcat(result, opcode);
        strcat(result, "000000");
        lineNumber++;
        return result;
    }
    else if(arguments == 1)
    {
        if(wordAmount != 2)
            printExtError(ERROR_CODE_47, (location){fileName, lineNumberSrcFile});

        char *operand = parsedLine[1];
        int addressingMethod = -1;
        char *addressing = getAddressingMethod(operand, *symbolTableHead, &addressingMethod);

        if(addressingMethod == -1)
            printIntError(ERROR_CODE_11);

        if(addressingMethod == 2)
        {
            /* get the index from the operand <label>[<index>] */
            char *inBrackets = (char *)calloc(strlen(operand), sizeof(char));
            if(inBrackets == NULL)
                printIntError(ERROR_CODE_10);

            int i = 0;
            for(i = 0; i < strlen(operand); i++)
            {
                if(operand[i] == '[')
                    break;
            }
            i++;
            int j = 0;
            for(; i < strlen(operand); i++)
            {
                if(operand[i] == ']')
                    break;
                inBrackets[j] = operand[i];
                j++;
            }

            /* get the label from the operand <label>[<index>] */
            char *beforeBrackets = (char *)calloc(strlen(operand), sizeof(char));
            if(beforeBrackets == NULL)
                printIntError(ERROR_CODE_10);

            for(i = 0; i < strlen(operand); i++)
            {
                if(operand[i] == '[')
                    break;
                beforeBrackets[i] = operand[i];
            }

            char *operandBinary = operandHandling(beforeBrackets, symbolTableHead, addressingMethod, 0, 0);

            int isConstant = 0;
            if(!isNumber(inBrackets))
                isConstant = 1;

            printf("In brackets: %s\n", inBrackets);
            printf("Before brackets: %s\n", beforeBrackets);

            char *indexBinary = operandHandling(inBrackets, symbolTableHead, -1, isConstant, 1);

            char *result = (char *)calloc(strlen(operandBinary) + strlen(indexBinary) + 17, sizeof(char));
            if(result == NULL)
                printIntError(ERROR_CODE_10);

            if(lineNumber == 1)
                strcat(result, "0000");
            else
                strcat(result, "\n0000");
            strcat(result, opcode);
            strcat(result, addressing);
            strcat(result, "0000\n");
            strcat(result, operandBinary);
            strcat(result, indexBinary);
            /* Remove the last \n */
            result[strlen(result) - 1] = '\0';

            free(operandBinary);
            free(indexBinary);

            lineNumber += 2;

            return result;
        }

        int isConstant = 0;
        /* isdigit only checks a char, the problem is the operand could be #-1 or #+1 and isdigit will say operand[1] isn't a digit */
        /* We'll get everything after the # and call isNumber on it */
        char *variable = (char *)calloc(strlen(operand), sizeof(char));
        if(variable == NULL)
            printIntError(ERROR_CODE_10);

        for(int i = 1; i < strlen(operand); i++)
            variable[i-1] = operand[i];
        if(addressingMethod == 0 && !isNumber(variable))
            isConstant = 1;

        free(variable);

        char *operandBinary = operandHandling(operand, symbolTableHead, addressingMethod, isConstant, 1);

        char *result = (char *)calloc(strlen(operandBinary) + 17, sizeof(char));
        if(result == NULL)
            printIntError(ERROR_CODE_10);

        if(lineNumber == 1)
            strcat(result, "0000");
        else
            strcat(result, "\n0000");
        strcat(result, opcode);
        strcat(result, "00");
        strcat(result, addressing);
        strcat(result, "00\n");
        strcat(result, operandBinary);
        /* Remove the last \n */
        result[strlen(result) - 1] = '\0';

        free(operandBinary);
        lineNumber += 2;
        return result;
    }
    else if(arguments == 2)
    {
        if(wordAmount != 3)
        {
            printf("Word amount: %d\n", wordAmount);
            printIntError(ERROR_CODE_31);
        }

        char *sourceOperand = parsedLine[1];
        char *destinationOperand = parsedLine[2];

        int sourceAddressingMethod = -1;
        char *sourceAddressing = getAddressingMethod(sourceOperand, *symbolTableHead, &sourceAddressingMethod);

        int destinationAddressingMethod = -1;
        char *destinationAddressing = getAddressingMethod(destinationOperand, *symbolTableHead, &destinationAddressingMethod);

        if(sourceAddressingMethod == -1 || destinationAddressingMethod == -1)
            printIntError(ERROR_CODE_11);

        if(sourceAddressingMethod == 3 && destinationAddressingMethod == 3)
        {
            char *regs = handleTwoRegisters(sourceOperand, destinationOperand);
            char *result = (char *)calloc(strlen(regs) + 17, sizeof(char));
            if(result == NULL)
                printIntError(ERROR_CODE_10);

            if(lineNumber == 1)
                strcat(result, "0000");
            else
                strcat(result, "\n0000");
            strcat(result, opcode);
            strcat(result, sourceAddressing);
            strcat(result, destinationAddressing);
            strcat(result, "00\n");
            strcat(result, regs);
            /* Remove the last \n */
            result[strlen(result) - 1] = '\0';

            lineNumber += 2;

            free(regs);
            return result;
        }
        else if(sourceAddressingMethod == 2 && destinationAddressingMethod == 2)
        {
            /* get the index from the operand <label>[<index>] */
            char *inBracketsSrc = (char *)calloc(strlen(sourceOperand), sizeof(char));
            if(inBracketsSrc == NULL)
                printIntError(ERROR_CODE_10);

            int i = 0;
            for(i = 0; i < strlen(sourceOperand); i++)
                if(sourceOperand[i] == '[')
                    break;
            i++;
            int j = 0;
            for(; i < strlen(sourceOperand); i++)
            {
                if(sourceOperand[i] == ']')
                    break;
                inBracketsSrc[j] = sourceOperand[i];
                j++;
            }

            /* get the label from the operand <label>[<index>] */
            char *beforeBracketsSrc = (char *)calloc(strlen(sourceOperand), sizeof(char));
            if(beforeBracketsSrc == NULL)
                printIntError(ERROR_CODE_10);

            for(i = 0; i < strlen(sourceOperand); i++)
            {
                if(sourceOperand[i] == '[')
                    break;
                beforeBracketsSrc[i] = sourceOperand[i];
            }

            char *operandBinarySrc = operandHandling(beforeBracketsSrc, symbolTableHead, sourceAddressingMethod, 0, 1);

            int isConstant = 0;
            if(!isNumber(inBracketsSrc))
                isConstant = 1;

            char *indexBinarySrc = operandHandling(inBracketsSrc, symbolTableHead, -1, isConstant, 1);

            /* get the index from the operand <label>[<index>] */
            char *inBracketsDest = (char *)calloc(strlen(destinationOperand), sizeof(char));
            if(inBracketsDest == NULL)
                printIntError(ERROR_CODE_10);

            i = 0;
            for(i = 0; i < strlen(destinationOperand); i++)
                if(destinationOperand[i] == '[')
                    break;

            i++;
            j = 0;
            for(; i < strlen(destinationOperand); i++)
            {
                if(destinationOperand[i] == ']')
                    break;
                inBracketsDest[j] = destinationOperand[i];
                j++;
            }

            /* get the label from the operand <label>[<index>] */
            char *beforeBracketsDest = (char *)calloc(strlen(destinationOperand), sizeof(char));

            if(beforeBracketsDest == NULL)
                printIntError(ERROR_CODE_10);

            for(i = 0; i < strlen(destinationOperand); i++)
            {
                if(destinationOperand[i] == '[')
                    break;
                beforeBracketsDest[i] = destinationOperand[i];
            }

            char *operandBinaryDest = operandHandling(beforeBracketsDest, symbolTableHead, destinationAddressingMethod, 0, 0);

            isConstant = 0;
            if(!isNumber(inBracketsDest))
                isConstant = 1;

            char *indexBinaryDest = operandHandling(inBracketsDest, symbolTableHead, -1, isConstant, 0);

            char *result = (char *)calloc(strlen(operandBinarySrc) + strlen(indexBinarySrc) + strlen(operandBinaryDest) + strlen(indexBinaryDest) + 17, sizeof(char));
            if(result == NULL)
                printIntError(ERROR_CODE_10);

            if(lineNumber == 1)
                strcat(result, "0000");
            else
                strcat(result, "\n0000");
            strcat(result, opcode);
            strcat(result, sourceAddressing);
            strcat(result, destinationAddressing);
            strcat(result, "00\n");
            strcat(result, operandBinarySrc);
            strcat(result, indexBinarySrc);
            strcat(result, operandBinaryDest);
            strcat(result, indexBinaryDest);
            /* Remove the last \n */
            result[strlen(result) - 1] = '\0';

            free(operandBinarySrc);
            free(indexBinarySrc);
            free(operandBinaryDest);
            free(indexBinaryDest);

            lineNumber += 3;

            return result;
        }
        else if(sourceAddressingMethod == 2)
        {
            /* get the index from the operand <label>[<index>] */
            char *inBrackets = (char *)calloc(strlen(sourceOperand), sizeof(char));
            if(inBrackets == NULL)
                printIntError(ERROR_CODE_10);

            int i = 0;
            for(i = 0; i < strlen(sourceOperand); i++)
                if(sourceOperand[i] == '[')
                    break;
            i++;
            int j = 0;
            for(; i < strlen(sourceOperand); i++)
            {
                if(sourceOperand[i] == ']')
                    break;
                inBrackets[j] = sourceOperand[i];
                j++;
            }

            /* get the label from the operand <label>[<index>] */
            char *beforeBrackets = (char *)calloc(strlen(sourceOperand), sizeof(char));
            if(beforeBrackets == NULL)
                printIntError(ERROR_CODE_10);

            for(i = 0; i < strlen(sourceOperand); i++)
            {
                if(sourceOperand[i] == '[')
                    break;
                beforeBrackets[i] = sourceOperand[i];
            }

            char *operandBinary = operandHandling(beforeBrackets, symbolTableHead, sourceAddressingMethod, 0, 1);

            int isConstant = 0;
            if(!isNumber(inBrackets))
                isConstant = 1;

            char *indexBinary = operandHandling(inBrackets, symbolTableHead, -1, isConstant, 1);

            char *result = (char *)calloc(strlen(operandBinary) + strlen(indexBinary) + 17, sizeof(char));
            if(result == NULL)
                printIntError(ERROR_CODE_10);

            if(lineNumber == 1)
                strcat(result, "0000");
            else
                strcat(result, "\n0000");
            strcat(result, opcode);
            strcat(result, sourceAddressing);
            strcat(result, destinationAddressing);
            strcat(result, "00\n");
            strcat(result, operandBinary);
            strcat(result, indexBinary);
            /* Remove the last \n */
            result[strlen(result) - 1] = '\0';

            free(operandBinary);
            free(indexBinary);

            lineNumber += 3;

            return result;
        }
        else if(destinationAddressingMethod == 2)
        {
            /* get the index from the operand <label>[<index>] */
            char *inBrackets = (char *)calloc(strlen(destinationOperand), sizeof(char));
            if(inBrackets == NULL)
                printIntError(ERROR_CODE_10);

            int i = 0;
            for(i = 0; i < strlen(destinationOperand); i++)
                if(destinationOperand[i] == '[')
                    break;
            i++;
            int j = 0;
            for(; i < strlen(destinationOperand); i++)
            {
                if(destinationOperand[i] == ']')
                    break;
                inBrackets[j] = destinationOperand[i];
                j++;
            }

            /* get the label from the operand <label>[<index>] */
            char *beforeBrackets = (char *)calloc(strlen(destinationOperand), sizeof(char));
            if(beforeBrackets == NULL)
                printIntError(ERROR_CODE_10);

            for(i = 0; i < strlen(destinationOperand); i++)
            {
                if(destinationOperand[i] == '[')
                    break;
                beforeBrackets[i] = destinationOperand[i];
            }

            char *operandBinary = operandHandling(beforeBrackets, symbolTableHead, destinationAddressingMethod, 0, 0);

            int isConstant = 0;
            if(!isNumber(inBrackets))
                isConstant = 1;

            char *indexBinary = operandHandling(inBrackets, symbolTableHead, -1, isConstant, 0);

            char *result = (char *)calloc(strlen(operandBinary) + strlen(indexBinary) + 17, sizeof(char));
            if(result == NULL)
                printIntError(ERROR_CODE_10);

            if(lineNumber == 1)
                strcat(result, "0000");
            else
                strcat(result, "\n0000");
            strcat(result, opcode);
            strcat(result, sourceAddressing);
            strcat(result, destinationAddressing);
            strcat(result, "00\n");
            strcat(result, operandBinary);
            strcat(result, indexBinary);
            /* Remove the last \n */
            result[strlen(result) - 1] = '\0';

            free(operandBinary);
            free(indexBinary);

            lineNumber += 3;

            return result;
        }

        int isConstant = 0;
        char *variable = (char *)calloc(strlen(sourceOperand), sizeof(char));
        for(int i = 1; i < strlen(sourceOperand); i++)
            variable[i-1] = sourceOperand[i];
        if(sourceAddressingMethod == 0 && !isNumber(variable))
            isConstant = 1;

        free(variable);

        char *sourceOperandBinary = operandHandling(sourceOperand, symbolTableHead, sourceAddressingMethod, isConstant, 1);

        isConstant = 0;
        variable = (char *)calloc(strlen(destinationOperand), sizeof(char));
        for(int i = 1; i < strlen(destinationOperand); i++)
            variable[i-1] = destinationOperand[i];
        if(destinationAddressingMethod == 0 && !isNumber(variable))
            isConstant = 1;

        free(variable);

        char *destinationOperandBinary = operandHandling(destinationOperand, symbolTableHead, destinationAddressingMethod, isConstant, 0);

        char *result = (char *)calloc(strlen(sourceOperandBinary) + strlen(destinationOperandBinary) + 17, sizeof(char));
        if(result == NULL)
            printIntError(ERROR_CODE_10);
        

        if(lineNumber == 1)
            strcat(result, "0000");
        else
            strcat(result, "\n0000");
        strcat(result, opcode);
        strcat(result, sourceAddressing);
        strcat(result, destinationAddressing);
        strcat(result, "00\n");
        strcat(result, sourceOperandBinary);
        strcat(result, destinationOperandBinary);
        /* Remove the last \n */
        result[strlen(result) - 1] = '\0';

        free(sourceOperandBinary);
        free(destinationOperandBinary);

        lineNumber += 3;

        return result;
    }
}

/* Purpose is explained in the header file */
char* handleString(char *line) {
    int amountOfChars = 0;
    /* Loop through the string */
    for (int i = 0; i < strlen(line); i++)
    {
        if(line[i] == '"')
            continue;
        amountOfChars++;
    }

    int lengthOfBinaryLine = (amountOfChars+1) * 15; /* Calculate the length of the binary line */

    char *binaryLine = (char *)calloc(lengthOfBinaryLine+1, sizeof(char)); /* Create a new string to store the binary line */
    if(binaryLine == NULL)
        printIntError(ERROR_CODE_10); /* Print an error and return */

    /* 
    Loop through the string
    The first char is " and the last char is ", the loop should run from 1 to amountOfChars 
    */
    int i = 0;
    for (i = 1; i < amountOfChars+1; i++)
    {
        /* Convert the character to ascii and then to binary */
        int number = line[i];
        /* Convert the number to a binary string */
        char *binaryNumber = intToBinary(number, BITS_AMOUNT);
        /* Add the binary number to the binary line */
        if(lineNumber == 1)
            strcat(binaryLine, binaryNumber);
        else {
            strcat(binaryLine, "\n");
            strcat(binaryLine, binaryNumber);
        }

        lineNumber++;
        free(binaryNumber);
    }
    /* Code the null terminator */
    char *binaryNumber = intToBinary(0, 14);
    if(lineNumber == 1)
        strcat(binaryLine, binaryNumber);
    else {
        strcat(binaryLine, "\n");
        strcat(binaryLine, binaryNumber);
    }
    lineNumber++;
    free(binaryNumber);

    DC += amountOfChars + 1; /* Increment the data counter */

    /* Return the binary line */
    return binaryLine;
}

/* Purpose is explained in the header file */
char *handleData(char *parsedLine[], Node **symbolTableHead, int wordAmount)
{
    char *binaryLine = (char *)calloc(15, sizeof(char)); /* Create a new string to store the binary line */
    if(binaryLine == NULL)
        printIntError(ERROR_CODE_10);

    /* Loop through the parsed line */
    /* Run from 1 to wordAmount because the first word is .data */
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
            binaryLine = (char *)realloc(binaryLine, sizeof(char) * (strlen(binaryLine) + strlen(binaryNumber) + 2));
            if(lineNumber == 1)
                strcat(binaryLine, binaryNumber);
            else {
                strcat(binaryLine, "\n");
                strcat(binaryLine, binaryNumber);
            }
            lineNumber++;
            free(binaryNumber);
            printf("Binary line: %s\n", binaryLine);
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
                if(lineNumber == 1)
                    strcat(binaryLine, binaryNumber);
                else {
                    strcat(binaryLine, "\n");
                    strcat(binaryLine, binaryNumber);
                }
                lineNumber++;
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
void *handleLabel(char *label, Node **symbolTableHead, int type)
{
    switch (type)
    {
        /* If the type is an instruction */
        case INSTRUCTION:
            /* Add the label to the symbol table */
            addNode(symbolTableHead, label, "instruction", IC);
            break;
        /* If the type is a data type */
        case DATA:
        case STRING:
            /* Add the label to the symbol table */
            addNode(symbolTableHead, label, "data", DC);
            break;
    }
}

/* Purpose is explained in the header file */
void handleConstant(char **parsedLine, Node **symbolTableHead, int wordAmount)
{
    if(wordAmount != 4)
        printIntError(ERROR_CODE_31); /* Print an error and return */
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
void calcLength(char **parsedLine, int wordAmount)
{
    printf("Word amount: %d\n", wordAmount);
    for(int i = 0; i < wordAmount; i++)
    {
        
        printf("parsedLine[%d]: %s\n", i, parsedLine[i]);
        int addressingMethod = -1;
        char *addressing = getAddressingMethod(parsedLine[i], symbolTable, &addressingMethod);
        printf("Addressing method: %d\n", addressingMethod);
        if(addressingMethod != 2)
            IC += 1;
        else
            IC += 2;
    }
}

void executeFirstPass(char *file, char **outputFileName)
{
    fileName = file;
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
    char *cleanedLine = NULL; // Create a buffer to store the cleaned line
    char *binaryLine = NULL; // Create a buffer to store the binary line

    while(fgets(line, MAX_LINE_LENGTH, inputFile) != NULL) // Loop through the input file
    {
        cleanedLine = removeCommas(cleanLine(line)); // Clean the line
        printf("Cleaned line: %s\n", cleanedLine);
        binaryLine = checkLineType(cleanedLine); // Check the type of the line
        printf("Binary line: %s\n", binaryLine);

        printf("Line number: %d\n", lineNumber);

        if (strcmp(binaryLine, "ERROR") != 0 && strcmp(binaryLine, "CONSTANT") != 0)
        {
            fprintf(outputFile, "%s", binaryLine);
        }

        free(cleanedLine);
        free(binaryLine);

        lineNumberSrcFile++;
        printf("Line number: %d\n", lineNumber);
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