#include <stdio.h> /* used for file operations */
#include <stdlib.h> /* used for malloc */
#include <string.h>  /* used for strlen */
#include <ctype.h> /* used for isdigit */
#include "../Headers/firstPass.h" /* Include the header file with the first pass functions */
#include "../Headers/errors.h" /* Include the header file with the error codes */
#include "../Headers/globalVariables.h" /* Include the header file with the global variables */
#include "../Headers/utilities.h" /* Include the header file with the utility functions */
#include "../Headers/dataStructures.h" /* Include the header file with the data structures */

int IC = 0, DC = 0; /* Initialize the instruction counter and the data counter */
int lineNumber = 1; /* Initialize the line number */
int lineNumberSrcFile = 1; /* Initialize the line number of the source file */
Node *symbolTable = NULL; /* Create the symbol table */

location fileLoc;

typedef struct OPCODE {
    char *name;
    char *binary;
    int arguments;
} Opcode;

const Opcode opcodes[] = {
    {"mov", "0000", 2},
    {"cmp", "0001", 2},
    {"add", "0010", 2},
    {"sub", "0011", 2},
    {"not", "0100", 1},
    {"clr", "0101", 1},
    {"lea", "0110", 2},
    {"inc", "0111", 1},
    {"dec", "1000", 1},
    {"jmp", "1001", 1},
    {"bne", "1010", 1},
    {"red", "1011", 1},
    {"prn", "1100", 1},
    {"jsr", "1101", 1},
    {"rts", "1110", 0},
    {"hlt", "1111", 0}
};

/* Purpose is explained in the header file */
int isInstruction(char *word)
{
    int i = 0; /* Create a variable to save the index of the instruction */
    /* Loop through the instruction names */
    for(i = 0; i < OPCODES_COUNT; i++)
    {
        /* If the word is an instruction name */
        if(strcmp(word, opcodes[i].name) == 0)
            return i; /* Return the index of the instruction */
    }
    return -1; /* Return -1 if the word is not an instruction name */
}

/* Purpose is explained in the header file */
char *checkLineType(char *line)
{
    int wordAmount;
    char **parsedLine;
    int i = 0; /* Create a variable to save the index of the word */
    int j = 0; /* Create a variable to save the index of the char */
    char *label; /* Create a variable to save the label */
    char *restOfLine; /* Create a variable to save the rest of the line */
    char *binaryLine; /* Create a variable to save the binary line */
    int instructionIndex; /* Create a variable to save the index of the instruction */
    int isLabel; /* Create a flag to check if the first word is a label */
    /* Return an empty string if the line is empty - the executer will write nothing to the output file */
    if (line[0] == ';' || line[0] == '\n')
        return "";

    wordAmount = countWords(line); /* Count the words in the line */

    if (wordAmount == 0) /* If the are no words in the line */
        printIntError(ERROR_CODE_45); /* Print an error */

    /* Parse the line */
    parsedLine = parseLine(line, wordAmount);
    /* run on the first word and check if there's a : in it - not at the end of the word 
    this means the line looks like <label>:<word> and we need to split it to two words */
    for(i = 0; i < strlen(parsedLine[0])-1; i++)
    {
        if(parsedLine[0][i] == ':')
        {
            /* Add one more word to the word amount */
            wordAmount++;
            /* save everything until the : including it in a string */
            label = (char *)calloc(i+1, sizeof(char));
            if (label == NULL)
                printIntError(ERROR_CODE_10);
            for(j = 0; j < i+1; j++)
                label[j] = parsedLine[0][j];

            /* save everything after the : in a string */
            restOfLine = (char *)calloc(strlen(parsedLine[0]) - i, sizeof(char));
            if (restOfLine == NULL)
                printIntError(ERROR_CODE_10);
            for(j = i+1; j < strlen(parsedLine[0]); j++)
                restOfLine[j-i-1] = parsedLine[0][j];

            /* Reallocate the parsed line to have one more word */
            parsedLine = (char **)realloc(parsedLine, sizeof(char *) * wordAmount);
            if (parsedLine == NULL)
                printIntError(ERROR_CODE_10);
            /* move all the words one place to the right */
            for(j = wordAmount-1; j > 1; j--)
            {
                parsedLine[j] = parsedLine[j-1];
            }
            /* Save the label and the rest of the line in the first two words */
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

    /* A variable to save the first word - only used if the first word is a label */
    label = (char *)calloc(strlen(parsedLine[0]), sizeof(char));
    /* Remove the : from the label */
    strcpy(label, parsedLine[0]);
    label[strlen(label)-1] = '\0';

    /* Create isLabel flag */
    isLabel = 0;
    if(parsedLine[0][strlen(parsedLine[0])-1] == ':')
    {
        isLabel = 1;
        /* Move all the words one place to the left - remove the label */
        for(i = 0; i < wordAmount; i++)
            parsedLine[i] = parsedLine[i+1];
        /* Decrement the word amount */
        wordAmount--;
    }

    /* Check if the first word in the line is .data - data declaration */
    if(strcmp(parsedLine[0], ".data") == 0)
    {
        /* If the first word is a label - add the label to the symbol table */
        if(isLabel)
        {
            handleLabel(label, &symbolTable, DATA);
        }
            
        /* Use handleData to translate the data to binary */
        binaryLine = handleData(parsedLine, &symbolTable, wordAmount);

        /* Word amount is the amount of words in the line not including the label
        we need to update DC by the amount of numbers / constants we added not including the label nor the .data */
        DC += (wordAmount - 1);

        /* Free the parsed line and return the result */
        free(parsedLine);
        return binaryLine;
    }
    /* Check if the first word in the line is .string - string declaration */
    else if(strcmp(parsedLine[0], ".string") == 0)
    {   
        /* If the first word is a label - add the label to the symbol table */
        if(isLabel){
            handleLabel(label, &symbolTable, STRING);
        }

        /* Use handleString to translate the string to binary */
        binaryLine = handleString(parsedLine[1]);

        /* Free the parsed line and return the result */
        free(parsedLine);
        return binaryLine;
    }
    /* Check if the first word in the line is .extern */
    else if(strcmp(parsedLine[0], ".extern") == 0)
    {
        i = 0;
        for(i = 1; i < wordAmount; i++)
            addNode(&symbolTable, parsedLine[i], "external", 0);
        free(parsedLine);
        return "";
    }

    if(isLabel)
        handleLabel(label, &symbolTable, INSTRUCTION);
    
    if(parsedLine[0][strlen(parsedLine[0])-1] == '\n')
        parsedLine[0][strlen(parsedLine[0])-1] = '\0';

    instructionIndex = isInstruction(parsedLine[0]);
    if(instructionIndex != -1)
    {
        /* Use handleInstruction to translate the instruction to binary */
        binaryLine = handleInstruction(parsedLine, &symbolTable, wordAmount);

        free(parsedLine);
        return binaryLine;
    }
    return "ERROR";
}

char *operandHandling(char *operand, Node **symbolTableHead, int addressingMethod, int isConstant, int isSource)
{
    char *binaryNumber;
    char *result;
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
        /* Remove the # from the operand */
        char *variable = (char *)calloc(strlen(operand), sizeof(char));
        int i = 0;
        for(i = 1; i < strlen(operand); i++)
            variable[i-1] = operand[i];

        if(variable[strlen(variable)-1] == '\n')
            variable[strlen(variable)-1] = '\0';

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
            /* If the constant is found in the symbol table */
            if (found == 1)
            {
                /* Convert the constant to a binary string */
                char *binaryNumber = intToBinary(node->line, 3);
                /* Add the binary number to the binary line */
                char *result = (char *)calloc(15, sizeof(char));
                if (result == NULL)
                    printIntError(ERROR_CODE_10);
                strcat(result, "000000000");
                strcat(result, binaryNumber);
                strcat(result, "00\n");
                free(binaryNumber);
                return result;
            }
            else
                printExtError(ERROR_CODE_46, fileLoc);
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
        if(registerNumber < 0 || registerNumber > 7 || strlen(operand) != 2)
            printExtError(ERROR_CODE_32, fileLoc);
        binaryNumber = intToBinary(registerNumber, 3);
        /* Add the binary number to the binary line */
        result = (char *)calloc(15, sizeof(char));
        if (result == NULL)
            printIntError(ERROR_CODE_10);
        if(isSource)
        {
            strcat(result, "000000");
            strcat(result, binaryNumber);
            strcat(result, "00000\n");
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
        if(isNumber(operand) == 1 && !isConstant)
        {
            /* Convert the index to an integer */
            int index = atoi(operand);
            /* Convert the index to a binary string */
            binaryNumber = intToBinary(index, 3);
            /* Add the binary number to the binary line */
            result = (char *)calloc(15, sizeof(char));
            if (result == NULL)
                printIntError(ERROR_CODE_10);
            strcat(result, "000000000");
            strcat(result, binaryNumber);
            strcat(result, "00\n");
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
                strcat(result, "000000000");
                strcat(result, binaryNumber);
                strcat(result, "00\n");
                free(binaryNumber);
                return result;
            }
            else
                printExtError(ERROR_CODE_46, fileLoc);
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
    char *sourceBinary;
    char *destinationBinary;
    char *result;
    /*
    The number of the source register will be coded at bits 5-7.
    The number of the destination register will be coded at bits 2-4.
    */
    int sourceRegisterNumber = soruceRegister[1] - '0';
    int destinationRegisterNumber = destinationRegister[1] - '0';

    if(sourceRegisterNumber < 0 || sourceRegisterNumber > 7)
        printExtError(ERROR_CODE_32, fileLoc);

    if(destinationRegisterNumber < 0 || destinationRegisterNumber > 7)
        printExtError(ERROR_CODE_32, fileLoc);

    sourceBinary = intToBinary(sourceRegisterNumber, 3);
    destinationBinary = intToBinary(destinationRegisterNumber, 3);
    result = (char *)calloc(15, sizeof(char));
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
    /* Initialize the variables - all of the same type in the same line */
    int instructionIndex, arguments, addressingMethod, sourceAddressingMethod, destinationAddressingMethod, isConstant;
    char *opcode, *result, *operand, *addressing, *inBrackets, *beforeBrackets, *operandBinary, *indexBinary, *variable, *sourceOperand, *destinationOperand;
    char *sourceAddressing, *destinationAddressing, *regs, *inBracketsSrc, *inBracketsDest, *beforeBracketsSrc, *beforeBracketsDest;
    char *operandBinarySrc, *operandBinaryDest, *indexBinarySrc, *indexBinaryDest;
    int i = 0, j = 0, originalLineNumber = lineNumber;

    /* First we'll get the instruction */
    instructionIndex = isInstruction(parsedLine[0]);
    /* Get the opcode */
    opcode = opcodes[instructionIndex].binary;
    /* Get the number of arguments */
    arguments = opcodes[instructionIndex].arguments;
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
            printExtError(ERROR_CODE_47, fileLoc);

        result = (char *)calloc(16, sizeof(char));
        if (result == NULL)
            printIntError(ERROR_CODE_10);
        if(lineNumber == 1)
            strcat(result, "0000");
        else
            strcat(result, "\n0000");
        strcat(result, opcode);
        strcat(result, "000000");
        lineNumber++;
        /* IC needs to be incremented by the amount of lines we added */
        IC += (lineNumber - originalLineNumber);

        return result;
    }
    else if(arguments == 1)
    {
        if(wordAmount != 2)
            printExtError(ERROR_CODE_47, fileLoc);

        operand = parsedLine[1];
        addressingMethod = -1;
        addressing = getAddressingMethod(operand, *symbolTableHead, &addressingMethod);

        if(addressingMethod == 2)
        {
            /* get the index from the operand <label>[<index>] */
            inBrackets = (char *)calloc(strlen(operand), sizeof(char));
            if(inBrackets == NULL)
                printIntError(ERROR_CODE_10);

            for(i = 0; i < strlen(operand); i++)
            {
                if(operand[i] == '[')
                    break;
            }
            i++;
            for(; i < strlen(operand); i++)
            {
                if(operand[i] == ']')
                    break;
                inBrackets[j] = operand[i];
                j++;
            }

            /* get the label from the operand <label>[<index>] */
            beforeBrackets = (char *)calloc(strlen(operand), sizeof(char));
            if(beforeBrackets == NULL)
                printIntError(ERROR_CODE_10);

            for(i = 0; i < strlen(operand); i++)
            {
                if(operand[i] == '[')
                    break;
                beforeBrackets[i] = operand[i];
            }

            operandBinary = operandHandling(beforeBrackets, symbolTableHead, addressingMethod, 0, 0);

            isConstant = 0;
            if(!isNumber(inBrackets))
                isConstant = 1;

            indexBinary = operandHandling(inBrackets, symbolTableHead, -1, isConstant, 1);

            result = (char *)calloc(strlen(operandBinary) + strlen(indexBinary) + 17, sizeof(char));
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

            lineNumber += 3;
            IC += (lineNumber - originalLineNumber);

            return result;
        }

        isConstant = 0;
        /* isdigit only checks a char, the problem is the operand could be #-1 or #+1 and isdigit will say operand[1] isn't a digit */
        /* We'll get everything after the # and call isNumber on it */
        variable = (char *)calloc(strlen(operand), sizeof(char));
        if(variable == NULL)
            printIntError(ERROR_CODE_10);

        for(i = 1; i < strlen(operand); i++)
            variable[i-1] = operand[i];
        if(addressingMethod == 0 && !isNumber(variable))
            isConstant = 1;
        free(variable);

        operandBinary = operandHandling(operand, symbolTableHead, addressingMethod, isConstant, 1);

        result = (char *)calloc(strlen(operandBinary) + 17, sizeof(char));
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
        IC += (lineNumber - originalLineNumber);
        return result;
    }
    else if(arguments == 2)
    {
        if(wordAmount != 3)
            printExtError(ERROR_CODE_47, fileLoc);

        sourceOperand = parsedLine[1];
        destinationOperand = parsedLine[2];

        sourceAddressingMethod = -1;
        sourceAddressing = getAddressingMethod(sourceOperand, *symbolTableHead, &sourceAddressingMethod);

        destinationAddressingMethod = -1;
        destinationAddressing = getAddressingMethod(destinationOperand, *symbolTableHead, &destinationAddressingMethod);

        if(sourceAddressingMethod == 3 && destinationAddressingMethod == 3)
        {
            regs = handleTwoRegisters(sourceOperand, destinationOperand);
            result = (char *)calloc(strlen(regs) + 17, sizeof(char));
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
            IC += (lineNumber - originalLineNumber);

            free(regs);
            return result;
        }
        else if(sourceAddressingMethod == 2 && destinationAddressingMethod == 2)
        {
            /* get the index from the operand <label>[<index>] */
            inBracketsSrc = (char *)calloc(strlen(sourceOperand), sizeof(char));
            if(inBracketsSrc == NULL)
                printIntError(ERROR_CODE_10);

            for(i = 0; i < strlen(sourceOperand); i++)
                if(sourceOperand[i] == '[')
                    break;
            i++;
            for(; i < strlen(sourceOperand); i++)
            {
                if(sourceOperand[i] == ']')
                    break;
                inBracketsSrc[j] = sourceOperand[i];
                j++;
            }

            /* get the label from the operand <label>[<index>] */
            beforeBracketsSrc = (char *)calloc(strlen(sourceOperand), sizeof(char));
            if(beforeBracketsSrc == NULL)
                printIntError(ERROR_CODE_10);

            for(i = 0; i < strlen(sourceOperand); i++)
            {
                if(sourceOperand[i] == '[')
                    break;
                beforeBracketsSrc[i] = sourceOperand[i];
            }

            operandBinarySrc = operandHandling(beforeBracketsSrc, symbolTableHead, sourceAddressingMethod, 0, 1);

            isConstant = 0;
            if(!isNumber(inBracketsSrc))
                isConstant = 1;

            indexBinarySrc = operandHandling(inBracketsSrc, symbolTableHead, -1, isConstant, 1);

            /* get the index from the operand <label>[<index>] */
            inBracketsDest = (char *)calloc(strlen(destinationOperand), sizeof(char));
            if(inBracketsDest == NULL)
                printIntError(ERROR_CODE_10);

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
            beforeBracketsDest = (char *)calloc(strlen(destinationOperand), sizeof(char));

            if(beforeBracketsDest == NULL)
                printIntError(ERROR_CODE_10);

            for(i = 0; i < strlen(destinationOperand); i++)
            {
                if(destinationOperand[i] == '[')
                    break;
                beforeBracketsDest[i] = destinationOperand[i];
            }

            operandBinaryDest = operandHandling(beforeBracketsDest, symbolTableHead, destinationAddressingMethod, 0, 0);

            isConstant = 0;
            if(!isNumber(inBracketsDest))
                isConstant = 1;

            indexBinaryDest = operandHandling(inBracketsDest, symbolTableHead, -1, isConstant, 0);

            result = (char *)calloc(strlen(operandBinarySrc) + strlen(indexBinarySrc) + strlen(operandBinaryDest) + strlen(indexBinaryDest) + 17, sizeof(char));
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

            lineNumber += 5;
            IC += (lineNumber - originalLineNumber);

            return result;
        }
        else if(sourceAddressingMethod == 2)
        {
            /* get the index from the operand <label>[<index>] */
            inBrackets = (char *)calloc(strlen(sourceOperand), sizeof(char));
            if(inBrackets == NULL)
                printIntError(ERROR_CODE_10);

            for(i = 0; i < strlen(sourceOperand); i++)
                if(sourceOperand[i] == '[')
                    break;
            i++;
            for(; i < strlen(sourceOperand); i++)
            {
                if(sourceOperand[i] == ']')
                    break;
                inBrackets[j] = sourceOperand[i];
                j++;
            }

            /* get the label from the operand <label>[<index>] */
            beforeBrackets = (char *)calloc(strlen(sourceOperand), sizeof(char));
            if(beforeBrackets == NULL)
                printIntError(ERROR_CODE_10);

            for(i = 0; i < strlen(sourceOperand); i++)
            {
                if(sourceOperand[i] == '[')
                    break;
                beforeBrackets[i] = sourceOperand[i];
            }

            operandBinarySrc = operandHandling(beforeBrackets, symbolTableHead, sourceAddressingMethod, 0, 1);

            isConstant = 0;
            if(!isNumber(inBrackets))
                isConstant = 1;

            indexBinarySrc = operandHandling(inBrackets, symbolTableHead, -1, isConstant, 1);

            isConstant = 0;
            variable = (char *)calloc(strlen(sourceOperand), sizeof(char));
            for(i = 1; i < strlen(destinationOperand); i++)
                variable[i-1] = destinationOperand[i];
            if(destinationAddressingMethod == 0 && !isNumber(variable))
                isConstant = 1;

            free(variable);

            operandBinaryDest = operandHandling(destinationOperand, symbolTableHead, destinationAddressingMethod, isConstant, 0);

            result = (char *)calloc(strlen(operandBinarySrc) + strlen(indexBinarySrc) + strlen(operandBinaryDest) + 17, sizeof(char));
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
            /* Remove the last \n */
            result[strlen(result) - 1] = '\0';

            free(operandBinarySrc);
            free(indexBinarySrc);
            free(operandBinaryDest);


            lineNumber += 4;
            IC += (lineNumber - originalLineNumber);

            return result;
        }
        else if(destinationAddressingMethod == 2)
        {
            /* Translate the source operand */
            isConstant = 0;
            variable = (char *)calloc(strlen(sourceOperand), sizeof(char));
            for(i = 1; i < strlen(sourceOperand); i++)
                variable[i-1] = sourceOperand[i];
            if(sourceAddressingMethod == 0 && !isNumber(variable))
                isConstant = 1;

            free(variable);

            operandBinarySrc = operandHandling(sourceOperand, symbolTableHead, sourceAddressingMethod, isConstant, 1);

            /* get the index from the operand <label>[<index>] */
            inBrackets = (char *)calloc(strlen(destinationOperand), sizeof(char));
            if(inBrackets == NULL)
                printIntError(ERROR_CODE_10);

            for(i = 0; i < strlen(destinationOperand); i++)
                if(destinationOperand[i] == '[')
                    break;
            i++;
            for(; i < strlen(destinationOperand); i++)
            {
                if(destinationOperand[i] == ']')
                    break;
                inBrackets[j] = destinationOperand[i];
                j++;
            }

            /* get the label from the operand <label>[<index>] */
            beforeBrackets = (char *)calloc(strlen(destinationOperand), sizeof(char));
            if(beforeBrackets == NULL)
                printIntError(ERROR_CODE_10);

            for(i = 0; i < strlen(destinationOperand); i++)
            {
                if(destinationOperand[i] == '[')
                    break;
                beforeBrackets[i] = destinationOperand[i];
            }

            isConstant = 0;
            if(!isNumber(inBrackets))
                isConstant = 1;

            indexBinaryDest = operandHandling(inBrackets, symbolTableHead, -1, isConstant, 1);
            operandBinaryDest = operandHandling(beforeBrackets, symbolTableHead, destinationAddressingMethod, 0, 0);

            result = (char *)calloc(strlen(operandBinarySrc) + strlen(operandBinaryDest) + strlen(indexBinaryDest) + 17, sizeof(char));
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
            strcat(result, operandBinaryDest);
            strcat(result, indexBinaryDest);
            /* Remove the last \n */
            result[strlen(result) - 1] = '\0';

            free(operandBinarySrc);
            free(operandBinaryDest);
            free(indexBinaryDest);

            lineNumber += 4;
            IC += (lineNumber - originalLineNumber);

            return result;
        }

        /* Translate the source operand */
        isConstant = 0;
        variable = (char *)calloc(strlen(sourceOperand), sizeof(char));
        for(i = 1; i < strlen(sourceOperand); i++)
            variable[i-1] = sourceOperand[i];
        if(sourceAddressingMethod == 0 && !isNumber(variable))
            isConstant = 1;

        free(variable);

        operandBinarySrc = operandHandling(sourceOperand, symbolTableHead, sourceAddressingMethod, isConstant, 1);

        /* Translate the destination operand */
        isConstant = 0;
        variable = (char *)calloc(strlen(destinationOperand), sizeof(char));
        for(i = 1; i < strlen(destinationOperand); i++)
            variable[i-1] = destinationOperand[i];
        if(destinationAddressingMethod == 0 && !isNumber(variable))
            isConstant = 1;

        free(variable);

        operandBinaryDest = operandHandling(destinationOperand, symbolTableHead, destinationAddressingMethod, isConstant, 0);

        result = (char *)calloc(strlen(operandBinarySrc) + strlen(operandBinaryDest) + 17, sizeof(char));
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
        strcat(result, operandBinaryDest);
        /* Remove the last \n */
        result[strlen(result) - 1] = '\0';

        free(operandBinarySrc);
        free(operandBinaryDest);

        lineNumber += 3;
        IC += (lineNumber - originalLineNumber);

        return result;
    }
    return "ERROR";
}

/* Purpose is explained in the header file */
char* handleString(char *line) {
    int amountOfChars = 0;
    int i = 0;
    int lengthOfBinaryLine = 0;
    char *binaryNumber, *binaryLine;
    /* Count the amount of chars in the string not including the "" */
    for (i = 1; i < strlen(line) - 1; i++)
        amountOfChars++;

    /* AmountOfChars is now too big by 1, we need to subtract 1 */
    amountOfChars--;

    lengthOfBinaryLine = (amountOfChars+1) * 15; /* Calculate the length of the binary line */

    binaryLine = (char *)calloc(lengthOfBinaryLine+1, sizeof(char)); /* Create a new string to store the binary line */
    if(binaryLine == NULL)
        printIntError(ERROR_CODE_10); /* Print an error and return */

    /* 
    Loop through the string
    The first char is " and the last char is ", the loop should run from 1 to amountOfChars 
    */
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

        free(binaryNumber);
    }
    /* Code the null terminator */
    binaryNumber = intToBinary(0, 14);
    if(lineNumber == 1)
        strcat(binaryLine, binaryNumber);
    else {
        strcat(binaryLine, "\n");
        strcat(binaryLine, binaryNumber);
    }
    free(binaryNumber);

    /* We need to update DC by the amount of chars we coded including the null terminator */
    DC += amountOfChars + 1;

    lineNumber += amountOfChars + 1; /* Increment the line number */

    /* Return the binary line */
    return binaryLine;
}

/* Purpose is explained in the header file */
char *handleData(char *parsedLine[], Node **symbolTableHead, int wordAmount)
{
    int i = 0, number = 0;
    char *binaryNumber;
    int found = 0;
    char *variable;
    char *binaryLine = (char *)calloc(15, sizeof(char)); /* Create a new string to store the binary line */
    if(binaryLine == NULL)
        printIntError(ERROR_CODE_10);

    /* Loop through the parsed line */
    /* Run from 1 to wordAmount because the first word is .data */
    for (i = 1; i < wordAmount; i++)
    {
        /* Check if the string is a number */
        if(isNumber(parsedLine[i]) == 1)
        {
            /* Convert the string to an integer */
            number = atoi(parsedLine[i]);
            /* Convert the number to a binary string */
            binaryNumber = intToBinary(number, BITS_AMOUNT);
            /* Add the binary number to the binary line */
            binaryLine = (char *)realloc(binaryLine, sizeof(char) * (strlen(binaryLine) + strlen(binaryNumber) + 2));
            if(lineNumber == 1)
                strcat(binaryLine, binaryNumber);
            else {
                strcat(binaryLine, "\n");
                strcat(binaryLine, binaryNumber);
            }
            free(binaryNumber);
        }
        else
        {
            Node *node;
            /* Check if the index is a constant in the symbol table */
            variable = (char *)calloc(strlen(parsedLine[i])-1, sizeof(char));
            if(variable == NULL)
                printIntError(ERROR_CODE_10);
            strcpy(variable, parsedLine[i]);
            if(variable[strlen(variable)-1] == '\n')
                variable[strlen(variable)-1] = '\0';
            found = 0;
            node = searchNodeInList(*symbolTableHead, variable, &found);
            /* If the constant is found in the symbol table */
            if (found == 1)
            {
                /* Convert the constant to a binary string */
                char *binaryNumber = intToBinary(node->line, BITS_AMOUNT);
                /* Add the binary number to the binary line */
                binaryLine = (char *)realloc(binaryLine, sizeof(char) * (strlen(binaryLine) + strlen(binaryNumber) + 2));
                if(lineNumber == 1)
                    strcat(binaryLine, binaryNumber);
                else {
                    strcat(binaryLine, "\n");
                    strcat(binaryLine, binaryNumber);
                }
                free(binaryNumber);
            }
            else
                printExtError(ERROR_CODE_46, fileLoc);
        }
    }
    lineNumber += wordAmount - 1; /* Increment the line number */
    /* Add a null terminator to the binary line */
    /* Return the binary line */
    return binaryLine;
}

/* Purpose is explained in the header file */
void handleLabel(char *label, Node **symbolTableHead, int type)
{
    switch (type)
    {
        /* If the type is an instruction */
        case INSTRUCTION:
            /* Add the label to the symbol table */
            addNode(symbolTableHead, label, "code", IC + 100);
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
    int found = 0;  
    if(wordAmount == 3)
        printExtError(ERROR_CODE_49, fileLoc);
    else if(wordAmount != 4)
        printExtError(ERROR_CODE_50, fileLoc);
    /* Check if the constant is a number */
    if (isNumber(parsedLine[3]) == 0)
    {
        printExtError(ERROR_CODE_51, fileLoc);
        return;
    }
    
    /* Make sure the constant is not already in the symbol table */
    found = 0;
    searchNodeInList(*symbolTableHead, parsedLine[1], &found);
    if (found == 1)
        printExtError(ERROR_CODE_48, fileLoc);
    /* Add the constant to the symbol table */
    else
        addNode(symbolTableHead, parsedLine[1], "mdefine", atoi(parsedLine[3]));
}

void executeFirstPass(char *file, char **outputFileName)
{
    char line[MAX_LINE_LENGTH + 1];
    char *cleanedLine;
    char *binaryLine;
    FILE *outputFile;
    Node *current;
    FILE *inputFile = fopen(file, "r");
    if (inputFile == NULL) 
        printIntError(ERROR_CODE_12);

    *outputFileName = calloc(strlen(file) + 2, sizeof(char));
    if(*outputFileName == NULL)
        printIntError(ERROR_CODE_10);
    strcpy(*outputFileName, file);
    /* Replace the file from <name>.txt\0 to <name>.obj\0 */
    (*outputFileName)[strlen(file) - 2] = 'o';
    (*outputFileName)[strlen(file)-1] = 'b';
    (*outputFileName)[strlen(file)] = 'j';
    (*outputFileName)[strlen(file) + 1] = '\0';

    outputFile = fopen(*outputFileName, "w");
    if (outputFile == NULL)
        printIntError(ERROR_CODE_13);
    
    fileLoc.fileName = file;

    while(fgets(line, MAX_LINE_LENGTH, inputFile) != NULL) 
    {
        cleanedLine = removeCommas(cleanLine(line));
        binaryLine = checkLineType(cleanedLine);

        if (strcmp(binaryLine, "ERROR") != 0 && strcmp(binaryLine, "CONSTANT") != 0)
        {
            fprintf(outputFile, "%s", binaryLine);
        }

        free(cleanedLine);
        free(binaryLine);

        lineNumberSrcFile++;
        fileLoc.line = lineNumberSrcFile;
    }

    fclose(inputFile);
    fclose(outputFile);

    /* Update every symbol that is a data type by adding IC + 100 */
    /* Run through the symbol table */
    current = symbolTable;

    /* IC will be wrong - it should be IC - 1 because the first line is 0 */

    while (current != NULL)
    {
        if(strcmp(current->data, "data") == 0)
            current->line = current->line + IC + 100;
        /* Move to the next node */
        current = current->next;
    }
}