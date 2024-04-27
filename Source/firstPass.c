#include <stdio.h> /* used for file operations */
#include <stdlib.h> /* used for malloc */
#include <string.h>  /* used for strlen */
#include <ctype.h> /* used for isdigit */
#include "../Headers/firstPass.h" /* Include the header file with the first pass functions */
#include "../Headers/errors.h" /* Include the header file with the error codes */
#include "../Headers/globalVariables.h" /* Include the header file with the global variables */
#include "../Headers/utilities.h" /* Include the header file with the utility functions */
#include "../Headers/dataStructures.h" /* Include the header file with the data structures */
#include "../Headers/secondPass.h" /* Include the header file with the second pass functions */

unsigned int IC = 0, DC = 0; /* Initialize the instruction counter and the data counter */
int lineNumber = 1; /* Initialize the line number */
int lineNumberSrcFile = 1; /* Initialize the line number of the source file */
int linesDataArraySize = 0; /* Initialize the size of the lines data array */
Node *symbolTable = NULL; /* Create the symbol table */
int noErrors = 1; /* Initialize the no errors flag */

lineData *linesDataArray;

location fileLoc;

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

int isOperandIllegal(char *operand)
{
    /* Check if the operand doesn't contain any illegal characters */
    int i;
    if(operand[strlen(operand)-1] == '\n')
        operand[strlen(operand)-1] = '\0';
    if(isdigit(operand[0]))
        return 1;
    for(i = 0; (unsigned)i < strlen(operand)-1; i++)
    {
        if(!isalpha(operand[i]) && !isdigit(operand[i]) && operand[i] != '[' && operand[i] != ']' && operand[i] != '#' && operand[i] != '-' && operand[i] != '+')
            return 1;
    }
    return 0;
}

int isLabelNameLegal(char *label)
{
    if(isdigit(label[0]))
        return 0;
    if(strlen(label) > MAX_LABEL_LENGTH)
        return 0;
    return 1;
}

/* Purpose is explained in the header file */
int isInstruction(char *word)
{
    int i = 0, j=0; /* Create a variable to save the index of the instruction */
    /* Loop through the instruction names */
	if(word[strlen(word)-1] == '\n') word[strlen(word)-1] = '\0';
	if(word[strlen(word)-1] == '\r') word[strlen(word)-1] = '\0';
    for(i = 0; i < OPCODES_COUNT; i++)
    {
		if(strcmp(word, opcodes[i].name) == 0) return i;
    }
    return -1; /* Return -1 if the word is not an instruction name */
}

/* Purpose is explained in the header file */
char *checkLineType(char *line, char *originalLine)
{
    int wordAmount, i = 0, j = 0, instructionIndex, isLabel, lineErrors = 1, tmpWordAmount;
    char **tmpParsedLine, *label, *restOfLine, *binaryLine, *firstVariable, **parsedLine = NULL;
    char *emptyReturn, *constantReturn, *labelPart;
    /* Return an empty string if the line is empty - the executer will write nothing to the output file */
    if (line[0] == ';' || line[0] == '\n' || line[0] == '\r' || line[0] == '\0')
    {
        emptyReturn = (char *)calloc(1, sizeof(char));
        if (emptyReturn == NULL){
            printIntError(ERROR_CODE_10);
            free(line);
            free(originalLine);
            exit(1);
        }
        return emptyReturn;
    }

    wordAmount = countWords(line); /* Count the words in the line */

    if (wordAmount == 0) /* If the are no words in the line */
    {
        printIntError(ERROR_CODE_45); /* Print an error message */
        free(line); /* Free the line */
        free(originalLine); /* Free the original line */
        exit(1); /* Exit the program */
    }

    /* Parse the line */
    
    tmpParsedLine = parseLine(line, wordAmount, &fileLoc, &lineErrors);
    
    if(lineErrors == 0)
    {
        for(i = 0; i < wordAmount; i++)
            free(parsedLine[i]);
        free(parsedLine);
        emptyReturn = (char *)calloc(1, sizeof(char));
        if (emptyReturn == NULL){
            printIntError(ERROR_CODE_10);
            free(line);
            free(originalLine);
            exit(1);
        }
        return emptyReturn;
    }
    isLegalCommas(originalLine, &noErrors, &fileLoc, tmpParsedLine, wordAmount);
    /* run on the first word and check if there's a : in it - not at the end of the word 
    this means the line looks like <label>:<word> and we need to split it to two words */
    for(i = 0; (unsigned)i < strlen(tmpParsedLine[0])-1; i++)
    {
        if(tmpParsedLine[0][i] == ':')
        {
            wordAmount++;
            
            labelPart = (char *)calloc(i+2, sizeof(char));
            if (labelPart == NULL)
            {
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(tmpParsedLine[i]);
                free(tmpParsedLine);
                free(line);
                free(originalLine);
                exit(1);
            }
            for(j = 0; j < i+1; j++)
                labelPart[j] = tmpParsedLine[0][j];

            labelPart[i+1] = '\0';

            restOfLine = (char *)calloc(strlen(tmpParsedLine[0])-i-1, sizeof(char));
            if (restOfLine == NULL)
            {
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(tmpParsedLine[i]);
                free(tmpParsedLine);
                free(line);
                free(originalLine);
                free(labelPart);
                exit(1);
            }
            for(j = i+1; (unsigned)j < strlen(tmpParsedLine[0])-1; j++)
                restOfLine[j-i-1] = tmpParsedLine[0][j];

            restOfLine[strlen(tmpParsedLine[0])-i-2] = '\0';

            parsedLine = (char **)calloc(wordAmount, sizeof(char *));
            if (parsedLine == NULL){
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);
                free(line);
                free(originalLine);
                free(labelPart);
                free(restOfLine);
                exit(1);
            }

            /* At the first word we need to add the label */
            parsedLine[0] = (char *)calloc(strlen(labelPart)+1, sizeof(char));
            if (parsedLine[0] == NULL){
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);
                free(line);
                free(originalLine);
                free(labelPart);
                free(restOfLine);
                exit(1);
            }
            strcpy(parsedLine[0], labelPart);

            parsedLine[1] = (char *)calloc(strlen(restOfLine)+1, sizeof(char));
            if (parsedLine[1] == NULL){
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);
                free(line);
                free(originalLine);
                free(labelPart);
                free(restOfLine);
                exit(1);
            }
            strcpy(parsedLine[1], restOfLine);

            /* And the rest of the words */
            for(j = 1; j < wordAmount-1; j++)
            {
                parsedLine[j+1] = (char *)calloc(strlen(tmpParsedLine[j])+1, sizeof(char));
                if (parsedLine[j+1] == NULL){
                    printIntError(ERROR_CODE_10);
                    for(i = 0; i < wordAmount; i++)
                        free(parsedLine[i]);
                    free(parsedLine);
                    free(line);
                    free(originalLine);
                    free(labelPart);
                    free(restOfLine);
                    exit(1);
                }
                strcpy(parsedLine[j+1], tmpParsedLine[j]);
            }
            
            for(i = 0; i < wordAmount; i++)
                free(tmpParsedLine[i]);
            free(tmpParsedLine);

            free(labelPart);
            free(restOfLine);
        }
    }

    if(parsedLine == NULL)
    {
        parsedLine = (char **)calloc(wordAmount, sizeof(char *));
        if (parsedLine == NULL){
            printIntError(ERROR_CODE_10);
            for(i = 0; i < wordAmount; i++)
                free(parsedLine[i]);
            free(parsedLine);
            free(line);
            free(originalLine);
            exit(1);
        }
        for(i = 0; i < wordAmount; i++)
        {
            parsedLine[i] = (char *)calloc(strlen(tmpParsedLine[i])+1, sizeof(char));
            if (parsedLine[i] == NULL){
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);
                free(line);
                free(originalLine);
                exit(1);
            }
            strcpy(parsedLine[i], tmpParsedLine[i]);
        }
        for(i = 0; i < wordAmount; i++)
            free(tmpParsedLine[i]);
        free(tmpParsedLine);
    }
    
    /* Check the first word in the line is .define - constant declaration */
    if(strcmp(parsedLine[0], ".define") == 0)
    {
        handleConstant(parsedLine, &symbolTable, wordAmount);
        /* Return nothing - the executer will write nothing to the output file */
        for(i = 0; i < wordAmount; i++)
            free(parsedLine[i]);
        free(parsedLine);

        constantReturn = (char *)calloc(9, sizeof(char));
        if (constantReturn == NULL){
            printIntError(ERROR_CODE_10);
            free(line);
            free(originalLine);
            exit(1);
        }
        strcpy(constantReturn, "CONSTANT\0");
        return constantReturn;
    }

    /* A variable to save the first word - only used if the first word is a label */
    label = (char *)calloc(strlen(parsedLine[0])+1, sizeof(char));
    if (label == NULL){
        printIntError(ERROR_CODE_10);
        for(i = 0; i < wordAmount; i++)
            free(parsedLine[i]);
        free(parsedLine);
        free(line);
        free(originalLine);
        exit(1);
    }
    /* Remove the : from the label */
    strcpy(label, parsedLine[0]);
    label[strlen(label)-1] = '\0';

    if(isLabelNameLegal(label) == 0)
    {
        printExtError(ERROR_CODE_30, fileLoc);
        noErrors = 0;
        for(i = 0; i < wordAmount; i++)
            free(parsedLine[i]);
        free(parsedLine);
        free(label);
        emptyReturn = (char *)calloc(1, sizeof(char));
        if (emptyReturn == NULL){
            printIntError(ERROR_CODE_10);
            free(line);
            free(originalLine);
            exit(1);
        }
        return emptyReturn;
    }

    /* Create isLabel flag */
    isLabel = 0;
    if(parsedLine[0][strlen(parsedLine[0])-1] == ':')
    {
        isLabel = 1;
        /* Move all the words one place to the left - remove the label */
        for(i = 0; i < wordAmount-1; i++)
        {
            free(parsedLine[i]);
			parsedLine[i] = NULL;
            if(parsedLine[i+1] == NULL){
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
				{
					if(parsedLine[i] != NULL)
						free(parsedLine[i]);
				}
                free(parsedLine);
                free(label);
                free(line);
                exit(1);
            }
            if(parsedLine[i+1][strlen(parsedLine[i+1])-1] == '\n')
                parsedLine[i+1][strlen(parsedLine[i+1])-1] = '\0';
            parsedLine[i] = (char *)calloc(strlen(parsedLine[i+1])+1, sizeof(char));
            if (parsedLine[i] == NULL){
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);
                free(label);
                free(line);
                free(originalLine);
                exit(1);
            }
            strcpy(parsedLine[i], parsedLine[i+1]);            
        }

        /* Free the last word */
        free(parsedLine[wordAmount-1]);
        /* Decrement the word amount */
        wordAmount--;
        /* Check if the length of the label is legal */
        if(strlen(label) > MAX_LABEL_LENGTH)
        {
            printExtError(ERROR_CODE_30, fileLoc);
            noErrors = 0;
            for(i = 0; i < wordAmount; i++)
                free(parsedLine[i]);
            free(parsedLine);
            free(label);
            emptyReturn = (char *)calloc(1, sizeof(char));
            if (emptyReturn == NULL){
                printIntError(ERROR_CODE_10);
                free(line);
                free(originalLine);
                exit(1);
            }
            return emptyReturn;
        }
    }

    firstVariable = (char *)calloc(strlen(parsedLine[0])+1, sizeof(char));
    if (firstVariable == NULL){
        printIntError(ERROR_CODE_10);
        for(i = 0; i < wordAmount; i++)
            free(parsedLine[i]);
        free(parsedLine);
        free(label);
        free(line);
        free(originalLine);
        exit(1);
    }
    strcpy(firstVariable, parsedLine[0]);
    if(firstVariable[strlen(firstVariable)-1] == '\n')
        firstVariable[strlen(firstVariable)-1] = '\0';


    /* Check if the first word in the line is .data - data declaration */
    if(strcmp(firstVariable, ".data") == 0)
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
        for(i = 0; i < wordAmount; i++)
            free(parsedLine[i]);
        free(parsedLine);
        free(label);
        free(firstVariable);
        return binaryLine;
    }
    /* Check if the first word in the line is .string - string declaration */
    else if(strcmp(firstVariable, ".string") == 0)
    {   
        /* If the first word is a label - add the label to the symbol table */
        tmpWordAmount = wordAmount;
        if(isLabel){
            tmpWordAmount--;
            handleLabel(label, &symbolTable, STRING);
        } 
        if(tmpWordAmount > 2){
            printExtError(ERROR_CODE_38, fileLoc);
            noErrors = 0;
            for(i = 0; i < wordAmount; i++)
                free(parsedLine[i]);
            free(parsedLine);
            free(label);
            free(firstVariable);
            emptyReturn = (char *)calloc(1, sizeof(char));
            if (emptyReturn == NULL){
                printIntError(ERROR_CODE_10);
                free(line);
                free(originalLine);
                exit(1);
            }
            return emptyReturn;
        }
        if(tmpWordAmount < 1){
            printExtError(ERROR_CODE_17, fileLoc);
            noErrors = 0;
            for(i = 0; i < wordAmount; i++)
                free(parsedLine[i]);
            free(parsedLine);
            free(label);
            free(firstVariable);
            emptyReturn = (char *)calloc(1, sizeof(char));
            if (emptyReturn == NULL){
                printIntError(ERROR_CODE_10);
                free(line);
                free(originalLine);
                exit(1);
            }
            return emptyReturn;
        }
        /* Use handleString to translate the string to binary */
        binaryLine = handleString(parsedLine, parsedLine[1], wordAmount);

        /* Free the parsed line and return the result */
        for(i = 0; i < wordAmount; i++)
            free(parsedLine[i]);
        free(parsedLine);
        free(label);
        free(firstVariable);
        return binaryLine;
    }
    /* Check if the first word in the line is .extern */
    else if(strcmp(firstVariable, ".extern") == 0)
    {
        i = 0;
        if(wordAmount != 2)
        {
            printExtError(ERROR_CODE_24, fileLoc);
            noErrors = 0;
            for(i = 0; i < wordAmount; i++)
                free(parsedLine[i]);
            free(parsedLine);
            free(label);
            free(firstVariable);
            emptyReturn = (char *)calloc(1, sizeof(char));
            if (emptyReturn == NULL){
                printIntError(ERROR_CODE_10);
                free(line);
                free(originalLine);
                exit(1);
            }
            return emptyReturn;
        }
        if(isLabelNameLegal(parsedLine[1]) == 0)
        {
            printExtError(ERROR_CODE_30, fileLoc);
            noErrors = 0;
            for(i = 0; i < wordAmount; i++)
                free(parsedLine[i]);
            free(parsedLine);
            free(label);
            free(firstVariable);
            emptyReturn = (char *)calloc(1, sizeof(char));
            if (emptyReturn == NULL){
                printIntError(ERROR_CODE_10);
                free(line);
                free(originalLine);
                exit(1);
            }
            return emptyReturn;
        }
        free(label);
        label = (char *)calloc(strlen(parsedLine[1])+1, sizeof(char));
        if(label == NULL){
            printIntError(ERROR_CODE_10);
            for(i = 0; i < wordAmount; i++)
                free(parsedLine[i]);
            free(parsedLine);
            free(firstVariable);
            exit(1);
        }
        strcpy(label, parsedLine[1]);
        if(label[strlen(label)-1] == '\n')
            label[strlen(label)-1] = '\0';
        addNode(&symbolTable, label, "external", 0);
        for(i = 0; i < wordAmount; i++)
            free(parsedLine[i]);
        free(parsedLine);
        free(label);
        free(firstVariable);
        emptyReturn = (char *)calloc(1, sizeof(char));
        if (emptyReturn == NULL){
            printIntError(ERROR_CODE_10);
            free(line);
            free(originalLine);
            exit(1);
        }
        return emptyReturn;
    }
    else if(strcmp(firstVariable, ".entry") == 0)
    {
        if(wordAmount != 2)
        {
            printExtError(ERROR_CODE_24, fileLoc);
            noErrors = 0;
        }
        if(isLabelNameLegal(parsedLine[1]) == 0)
        {
            printExtError(ERROR_CODE_30, fileLoc);
            noErrors = 0;
        }
        free(label);
        free(firstVariable);
        for(i = 0; i < wordAmount; i++)
            free(parsedLine[i]);
        free(parsedLine);
        emptyReturn = (char *)calloc(1, sizeof(char));
        if (emptyReturn == NULL){
            printIntError(ERROR_CODE_10);
            free(line);
            free(originalLine);
            exit(1);
        }
        return emptyReturn;
    }

    if(isLabel)
        handleLabel(label, &symbolTable, INSTRUCTION);

    free(label);

    instructionIndex = isInstruction(firstVariable);
    if(instructionIndex != -1)
    {
        /* Use handleInstruction to translate the instruction to binary */
        binaryLine = handleInstruction(parsedLine, &symbolTable, wordAmount);

        for(i = 0; i < wordAmount; i++)
            free(parsedLine[i]);
        free(parsedLine);
        free(firstVariable);
        return binaryLine;
    }
    printExtError(ERROR_CODE_26, fileLoc);
    noErrors = 0;
    for(i = 0; i < wordAmount; i++)
        free(parsedLine[i]);
    free(parsedLine);
    free(firstVariable);
    emptyReturn = (char *)calloc(1, sizeof(char));
    if (emptyReturn == NULL){
        printIntError(ERROR_CODE_10);
        free(line);
        free(originalLine);
        exit(1);
    }
    return emptyReturn;
}

char *operandHandling(char *operand, Node **symbolTableHead, int addressingMethod, int isConstant, int isSource, int *hasLabel)
{
    char *binaryNumber;
    char *result;
    char *emptyReturn;
    size_t totalLength = 0;
    int number, found;
    Node *node;
    char *variable;
    int i;
    int registerNumber;
    if(operand[0] == '#' && addressingMethod == 0)
    {
        /* Remove the # from the operand */
        variable = (char *)calloc(strlen(operand)+1, sizeof(char));
        if (variable == NULL){
            printIntError(ERROR_CODE_10);
            exit(1);
        }
        for(i = 1; (unsigned)i < strlen(operand); i++)
            variable[i-1] = operand[i];

        if(variable[strlen(variable)-1] == '\n')
            variable[strlen(variable)-1] = '\0';

        /* Check if the string is a number or a constant */
        if(isNumber(variable) == 1 && !isConstant)
        {
            /* Convert the string to an integer */
            number = atoi(variable);
            /* Convert the number to a binary string */
            binaryNumber = intToBinary(number, 12);
            /* Add the binary number to the binary line */
            totalLength = strlen(binaryNumber) + strlen("00") + 1;
            result = (char *)calloc(totalLength, sizeof(char));
            if (result == NULL){
                printIntError(ERROR_CODE_10);
                exit(1);
            }
            strcat(result, binaryNumber);
            strcat(result, "00\0");
            free(binaryNumber);
            free(variable);
            return result;
        }
        else if (isConstant)
        {
            /* Check if the index is a constant in the symbol table */
            found = 0;
            node = searchNodeInList(*symbolTableHead, variable, &found);
            /* If the constant is found in the symbol table */
            if (found == 1)
            {
                /* Convert the constant to a binary string */
                binaryNumber = intToBinary(node->line, 3);
                /* Add the binary number to the binary line */
                totalLength = strlen(binaryNumber) + strlen("00") + strlen("000000000") + 1;
                result = (char *)calloc(totalLength, sizeof(char));
                if (result == NULL){
                    printIntError(ERROR_CODE_10);
                    exit(1);
                }
                strcat(result, "000000000");
                strcat(result, binaryNumber);
                strcat(result, "00\0");
                free(binaryNumber);
                free(variable);
                return result;
            }
            else{
                free(variable);
                printExtError(ERROR_CODE_46, fileLoc);
                noErrors = 0;
                emptyReturn = (char *)calloc(1, sizeof(char));
                if (emptyReturn == NULL){
                    printIntError(ERROR_CODE_10);
                    exit(1);
                }
                return emptyReturn;
            }
        }
    }
    else if(addressingMethod == 1)
    {
        /* Search for the label in the symbol table - could be with the type "data" or "external"
        If the label is found in the symbol table with the type "data" - convert the label to a binary string and add 10 at the end
        If the label is found in the symbol table with the type "external" - convert the label to a binary string and add 01 at the end
        If the label is not found in the symbol table - second pass will handle it - return an empty string */
        found = 0;
        node = searchNodeInList(*symbolTableHead, operand, &found);
        *hasLabel = 1;
        if (found == 1 && strcmp(node->data, "data") == 0)
        {
            /* Convert the label to a binary string */
            binaryNumber = intToBinary(node->line, 12);
            /* Add the binary number to the binary line */
            totalLength = strlen(binaryNumber) + strlen("10") + 1;
            result = (char *)calloc(totalLength, sizeof(char));
            if (result == NULL){
                printIntError(ERROR_CODE_10);
                exit(1);
            }
            strcat(result, binaryNumber);
            strcat(result, "10\0");
            free(binaryNumber);
            return result;
        }
        else
        {
            emptyReturn = (char *)calloc(1, sizeof(char));
            if (emptyReturn == NULL){
                printIntError(ERROR_CODE_10);
                exit(1);
            }
            return emptyReturn;
        }
    }
    /* registers */
    else if (operand[0] == 'r' && addressingMethod == 3)
    {
        /* Get the register number */
		for(i = 0; i < strlen(operand);i++)
			printf("%d\n", operand[i]);
        registerNumber = 0;
        if(operand[strlen(operand)-1] == '\n')
            operand[strlen(operand)-1] = '\0';
		if(operand[strlen(operand)-1] == '\r')
            operand[strlen(operand)-1] = '\0';
        for(i = 1; (unsigned)i < strlen(operand); i++)
        {
            if(!isdigit(operand[i]))
            {
                printExtError(ERROR_CODE_32, fileLoc);
                noErrors = 0;
                emptyReturn = (char *)calloc(1, sizeof(char));
                if (emptyReturn == NULL){
                    printIntError(ERROR_CODE_10);
                    exit(1);
                }
                return emptyReturn;
            }
            registerNumber = registerNumber * 10 + (operand[i] - '0');
        }
        /* Convert the register number to a binary string */
        if(registerNumber < 0 || registerNumber > 7)
        {
            printExtError(ERROR_CODE_32, fileLoc);
            noErrors = 0;
            emptyReturn = (char *)calloc(1, sizeof(char));
            if (emptyReturn == NULL){
                printIntError(ERROR_CODE_10);
                exit(1);
            }
            return emptyReturn;
        }
        binaryNumber = intToBinary(registerNumber, 3);
        /* Add the binary number to the binary line */
        totalLength = strlen(binaryNumber) + strlen("000000") + strlen("00000") + 1;
        result = (char *)calloc(totalLength, sizeof(char));
        if (result == NULL){
            printIntError(ERROR_CODE_10);
            exit(1);
        }
        if(isSource)
        {
            strcat(result, "000000");
            strcat(result, binaryNumber);
            strcat(result, "00000\0");
        }
        else
        {
            strcat(result, "000000000");
            strcat(result, binaryNumber);
            strcat(result, "00\0");
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
            i = atoi(operand);
            /* Convert the index to a binary string */
            binaryNumber = intToBinary(i, 3);
            /* Add the binary number to the binary line */
            totalLength = strlen(binaryNumber) + strlen("00") + strlen("000000000") + 1;
            result = (char *)calloc(totalLength, sizeof(char));
            if (result == NULL){
                printIntError(ERROR_CODE_10);
                exit(1);
            }
            strcat(result, "000000000");
            strcat(result, binaryNumber);
            strcat(result, "00\0");
            free(binaryNumber);
            return result;
        }
        else if (isConstant)
        {
            /* Check if the index is a constant in the symbol table */
            found = 0;
            node = searchNodeInList(*symbolTableHead, operand, &found);
            /* If the constant is found in the symbol table */
            if (found == 1)
            {
                /* Convert the constant to a binary string */
                binaryNumber = intToBinary(node->line, 3);
                /* Add the binary number to the binary line */
                totalLength = strlen(binaryNumber) + strlen("00") + strlen("000000000") + 1;
                result = (char *)calloc(totalLength, sizeof(char));
                if (result == NULL){
                    printIntError(ERROR_CODE_10);
                    exit(1);
                }
                strcat(result, "000000000");
                strcat(result, binaryNumber);
                strcat(result, "00\0");
                free(binaryNumber);
                return result;
            }
            else
            {
                printExtError(ERROR_CODE_46, fileLoc);
                noErrors = 0;
                emptyReturn = (char *)calloc(1, sizeof(char));
                if (emptyReturn == NULL){
                    printIntError(ERROR_CODE_10);
                    exit(1);
                }
                return emptyReturn;
            }
        }
    }
    else if(addressingMethod == 2)
    {
        /* Check if the label is in the symbol table with the type "data" */
        found = 0;
        node = searchNodeInList(*symbolTableHead, operand, &found);
        /* If the label is found in the symbol table */
        if (found == 1)
        {
            /* Convert the label to a binary string */
            binaryNumber = intToBinary(node->line, 12);
            /* Add the binary number to the binary line */
            totalLength = strlen(binaryNumber) + strlen("10") + 1;
            result = (char *)calloc(totalLength, sizeof(char));
            if (result == NULL){
                printIntError(ERROR_CODE_10);
                exit(1);
            }
            strcat(result, binaryNumber);
            strcat(result, "10\0");
            free(binaryNumber);
            return result;
        }
        else
        {
            emptyReturn = (char *)calloc(1, sizeof(char));
            if (emptyReturn == NULL){
                printIntError(ERROR_CODE_10);
                exit(1);
            }
            return emptyReturn;
        }
    }

    emptyReturn = (char *)calloc(1, sizeof(char));
    if (emptyReturn == NULL){
        printIntError(ERROR_CODE_10);
        exit(1);
    }
    return emptyReturn;
}

char *handleTwoRegisters(char *soruceRegister, char *destinationRegister)
{
    char *sourceBinary;
    char *destinationBinary;
    char *result;
    char *emptyReturn;
    size_t totalLength;
    /*
    The number of the source register will be coded at bits 5-7.
    The number of the destination register will be coded at bits 2-4.
    */
    int sourceRegisterNumber = soruceRegister[1] - '0';
    int destinationRegisterNumber = destinationRegister[1] - '0';

    if(sourceRegisterNumber < 0 || sourceRegisterNumber > 7){
        printExtError(ERROR_CODE_32, fileLoc);
        noErrors = 0;
        emptyReturn = (char *)calloc(1, sizeof(char));
        if (emptyReturn == NULL){
            printIntError(ERROR_CODE_10);
            exit(1);
        }
        return emptyReturn;
    }

    if(destinationRegisterNumber < 0 || destinationRegisterNumber > 7){
        printExtError(ERROR_CODE_32, fileLoc);
        noErrors = 0;
        emptyReturn = (char *)calloc(1, sizeof(char));
        if (emptyReturn == NULL){
            printIntError(ERROR_CODE_10);
            exit(1);
        }
        return emptyReturn;
    }

    sourceBinary = intToBinary(sourceRegisterNumber, 3);
    destinationBinary = intToBinary(destinationRegisterNumber, 3);
    totalLength = strlen(sourceBinary) + strlen(destinationBinary) + strlen("000000") + strlen("00") + 1;
    result = (char *)calloc(totalLength, sizeof(char));
    if (result == NULL){
        printIntError(ERROR_CODE_10);
        exit(1);
    }
        
    strcat(result, "000000");
    strcat(result, sourceBinary);
    strcat(result, destinationBinary);
    strcat(result, "00\0");

    printf("sourceBinary: %s\n", sourceBinary);
    printf("destinationBinary: %s\n", destinationBinary);
    printf("result: %s\n", result);

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
    char *operandBinarySrc, *operandBinaryDest, *indexBinarySrc, *indexBinaryDest, *emptyReturn;
    int i = 0, j = 0, originalLineNumber = lineNumber;
    int hasLabel = 0, srcHasLabel = 0, destHasLabel = 0;
    size_t totalLength;

    /* First we'll get the instruction */
    instructionIndex = isInstruction(parsedLine[0]);
    /* Get the opcode */
    opcode = opcodes[instructionIndex].binary;
    /* Get the number of arguments */
    arguments = opcodes[instructionIndex].arguments;
    
    if(arguments == 0)
    {
        /* make sure parsed line has only the instruction - no operands */
        if(wordAmount != 1)
        {
            printExtError(ERROR_CODE_47, fileLoc);
            noErrors = 0;
            emptyReturn = (char *)calloc(1, sizeof(char));
            if (emptyReturn == NULL){
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);
                exit(1);
            }
            return emptyReturn;
        }
        
        totalLength = strlen(opcode) + strlen("0000") + strlen("000000") + 1;
        result = (char *)calloc(totalLength, sizeof(char));
        if (result == NULL)
        {
            printIntError(ERROR_CODE_10);
            for(i = 0; i < wordAmount; i++)
                free(parsedLine[i]);
            free(parsedLine);
            exit(1);
        }
        if(lineNumber == 1)
            strcat(result, "0000");
        else
        {
            totalLength++;
            result = (char *)realloc(result, totalLength);
            if (result == NULL)
            {
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);
                exit(1);
            }
            strcat(result, "\n0000");
        }
        strcat(result, opcode);
        strcat(result, "000000");
        lineNumber++;
        /* IC needs to be incremented by the amount of lines we added */
        IC += (lineNumber - originalLineNumber);

        addLine(&linesDataArray, 1, -1, -1, &linesDataArraySize);

        return result;
    }
    else if(arguments == 1)
    {
        if(wordAmount != 2)
        {
            printExtError(ERROR_CODE_47, fileLoc);
            noErrors = 0;
            emptyReturn = (char *)calloc(1, sizeof(char));
            if (emptyReturn == NULL){
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);   
                exit(1);
            }
            return emptyReturn;
        }

        operand = parsedLine[1];
        if(isOperandIllegal(operand)){
            printExtError(ERROR_CODE_21, fileLoc);
            noErrors = 0;
            emptyReturn = (char *)calloc(1, sizeof(char));
            if (emptyReturn == NULL){
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);
                exit(1);
            }
            return emptyReturn;
        }
        addressingMethod = -1;
        addressing = getAddressingMethod(operand, *symbolTableHead, &addressingMethod);

        if(addressingMethod == 2)
        {
            /* get the index from the operand <label>[<index>] */
            inBrackets = (char *)calloc(strlen(operand), sizeof(char));
            if(inBrackets == NULL){
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);
                exit(1);
            }

            for(i = 0; (unsigned)i < strlen(operand); i++)
            {
                if(operand[i] == '[')
                    break;
            }
            i++;
            for(; (unsigned)i < strlen(operand); i++)
            {
                if(operand[i] == ']')
                    break;
                inBrackets[j] = operand[i];
                j++;
            }

            /* get the label from the operand <label>[<index>] */
            beforeBrackets = (char *)calloc(strlen(operand), sizeof(char));
            if(beforeBrackets == NULL){
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);
                exit(1);
            }

            for(i = 0; (unsigned)i < strlen(operand); i++)
            {
                if(operand[i] == '[')
                    break;
                beforeBrackets[i] = operand[i];
            }

            operandBinary = operandHandling(beforeBrackets, symbolTableHead, addressingMethod, 0, 0, &hasLabel);
            free(beforeBrackets);

            isConstant = 0;
            if(!isNumber(inBrackets))
                isConstant = 1;

            indexBinary = operandHandling(inBrackets, symbolTableHead, -1, isConstant, 1, &hasLabel);
            free(inBrackets);

            totalLength = strlen("0000") + strlen(opcode) + strlen(addressing) + strlen("0000\n") + strlen(operandBinary) + strlen("\n") + strlen(indexBinary) + 1;
            result = (char *)calloc(totalLength, sizeof(char));
            if(result == NULL)
            {
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);
                exit(1);
            }

            if(lineNumber == 1)
                strcat(result, "0000");
            else
            {
                totalLength++;
                result = (char *)realloc(result, totalLength);
                if(result == NULL)
                {
                    printIntError(ERROR_CODE_10);
                    for(i = 0; i < wordAmount; i++)
                        free(parsedLine[i]);
                    free(parsedLine);
                    exit(1);
                }
                strcat(result, "\n0000");
            }
            strcat(result, opcode);
            strcat(result, addressing);
            strcat(result, "0000\n");
            strcat(result, operandBinary);
            /* Add a new line character after the operand */
            strcat(result, "\n");
            strcat(result, indexBinary);

            free(operandBinary);
            free(indexBinary);

            lineNumber += 3;
            IC += (lineNumber - originalLineNumber);

            addLine(&linesDataArray, 3, 2, -1, &linesDataArraySize);

            return result;
        }

        isConstant = 0;
        /* isdigit only checks a char, the problem is the operand could be #-1 or #+1 and isdigit will say operand[1] isn't a digit */
        /* We'll get everything after the # and call isNumber on it */
        variable = (char *)calloc(strlen(operand), sizeof(char));
        if(variable == NULL){
            printIntError(ERROR_CODE_10);
            for(i = 0; i < wordAmount; i++)
                free(parsedLine[i]);
            free(parsedLine);
            exit(1);
        }

        for(i = 1;(unsigned) i < strlen(operand); i++)
            variable[i-1] = operand[i];
        if(addressingMethod == 0 && !isNumber(variable))
            isConstant = 1;

        free(variable);
        
        hasLabel = 0;
        operandBinary = operandHandling(operand, symbolTableHead, addressingMethod, isConstant, 1, &hasLabel);
        
        totalLength = strlen("0000") + strlen(opcode) + strlen("00") + strlen(addressing) + strlen("00\n") + strlen(operandBinary) + 1;
        result = (char *)calloc(totalLength, sizeof(char));
        if(result == NULL){
            printIntError(ERROR_CODE_10);
            for(i = 0; i < wordAmount; i++)
                free(parsedLine[i]);
            free(parsedLine);
            exit(1);
        }

        if(lineNumber == 1)
            strcat(result, "0000");
        else
        {
            totalLength++;
            result = (char *)realloc(result, totalLength);
            if(result == NULL){
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);
                exit(1);
            }
            strcat(result, "\n0000");
        }
        strcat(result, opcode);
        strcat(result, "00");
        strcat(result, addressing);
        strcat(result, "00\n");
        strcat(result, operandBinary);

        free(operandBinary);
        lineNumber += 2;
        IC += (lineNumber - originalLineNumber);

        if(hasLabel){
            addLine(&linesDataArray, 2, 2, -1, &linesDataArraySize);
        }
        else{
            addLine(&linesDataArray, 2, -1, -1, &linesDataArraySize);
        }

        return result;
    }
    else if(arguments == 2)
    {
        if(wordAmount != 3)
        {
            printExtError(ERROR_CODE_47, fileLoc);
            noErrors = 0;
            emptyReturn = (char *)calloc(1, sizeof(char));
            if (emptyReturn == NULL){
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);
                exit(1);
            }
            return emptyReturn;
        }

        sourceOperand = (char *)calloc(strlen(parsedLine[1])+1, sizeof(char));
        if(sourceOperand == NULL){
            printIntError(ERROR_CODE_10);
            for(i = 0; i < wordAmount; i++)
                free(parsedLine[i]);
            free(parsedLine);
            exit(1);
        }
        strcpy(sourceOperand, parsedLine[1]);

        destinationOperand = (char *)calloc(strlen(parsedLine[2])+1, sizeof(char));
        if(destinationOperand == NULL){
            printIntError(ERROR_CODE_10);
            for(i = 0; i < wordAmount; i++)
                free(parsedLine[i]);
            free(parsedLine);
            exit(1);
        }
        strcpy(destinationOperand, parsedLine[2]);

        sourceAddressingMethod = -1;
        sourceAddressing = getAddressingMethod(sourceOperand, *symbolTableHead, &sourceAddressingMethod);

        destinationAddressingMethod = -1;
        destinationAddressing = getAddressingMethod(destinationOperand, *symbolTableHead, &destinationAddressingMethod);

        if(sourceAddressingMethod == 3 && destinationAddressingMethod == 3)
        {
            regs = handleTwoRegisters(sourceOperand, destinationOperand);
            printf("regs: %s\n", regs);
            totalLength = strlen("0000") + strlen(opcode) + strlen(sourceAddressing) + strlen(destinationAddressing) + strlen("00\n") + strlen(regs) + 1;
            result = (char *)calloc(totalLength, sizeof(char));
            if(result == NULL){
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);
                exit(1);
            }

            if(lineNumber == 1)
                strcat(result, "0000");
            else
            {
                totalLength++;
                result = (char *)realloc(result, totalLength);
                if(result == NULL){
                    printIntError(ERROR_CODE_10);
                    for(i = 0; i < wordAmount; i++)
                        free(parsedLine[i]);
                    free(parsedLine);
                    exit(1);
                }
                strcat(result, "\n0000");
            }
            strcat(result, opcode);
            strcat(result, sourceAddressing);
            strcat(result, destinationAddressing);
            strcat(result, "00\n");
            strcat(result, regs);

            lineNumber += 2;
            IC += (lineNumber - originalLineNumber);

            free(regs);
            free(sourceOperand);
            free(destinationOperand);

            addLine(&linesDataArray, 2, -1, -1, &linesDataArraySize);

            return result;
        }
        else if(sourceAddressingMethod == 2 && destinationAddressingMethod == 2)
        {
            /* get the index from the operand <label>[<index>] */
            inBracketsSrc = (char *)calloc(strlen(sourceOperand), sizeof(char));
            if(inBracketsSrc == NULL){
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);
                exit(1);
            }

            for(i = 0; (unsigned)i < strlen(sourceOperand); i++)
                if(sourceOperand[i] == '[')
                    break;
            i++;
            for(; (unsigned)i < strlen(sourceOperand); i++)
            {
                if(sourceOperand[i] == ']')
                    break;
                inBracketsSrc[j] = sourceOperand[i];
                j++;
            }

            /* get the label from the operand <label>[<index>] */
            beforeBracketsSrc = (char *)calloc(strlen(sourceOperand), sizeof(char));
            if(beforeBracketsSrc == NULL){
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);
                exit(1);
            }

            for(i = 0; (unsigned)i < strlen(sourceOperand); i++)
            {
                if(sourceOperand[i] == '[')
                    break;
                beforeBracketsSrc[i] = sourceOperand[i];
            }

            operandBinarySrc = operandHandling(beforeBracketsSrc, symbolTableHead, sourceAddressingMethod, 0, 1, &hasLabel);
            free(beforeBracketsSrc);

            isConstant = 0;
            if(!isNumber(inBracketsSrc))
                isConstant = 1;

            indexBinarySrc = operandHandling(inBracketsSrc, symbolTableHead, -1, isConstant, 1, &hasLabel);
            free(inBracketsSrc);

            /* get the index from the operand <label>[<index>] */
            inBracketsDest = (char *)calloc(strlen(destinationOperand), sizeof(char));
            if(inBracketsDest == NULL){
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);
                exit(1);
            }

            for(i = 0; (unsigned)i < strlen(destinationOperand); i++)
                if(destinationOperand[i] == '[')
                    break;

            i++;
            j = 0;
            for(; (unsigned)i < strlen(destinationOperand); i++)
            {
                if(destinationOperand[i] == ']')
                    break;
                inBracketsDest[j] = destinationOperand[i];
                j++;
            }

            /* get the label from the operand <label>[<index>] */
            beforeBracketsDest = (char *)calloc(strlen(destinationOperand), sizeof(char));

            if(beforeBracketsDest == NULL){
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);
                exit(1);
            }

            for(i = 0; (unsigned)i < strlen(destinationOperand); i++)
            {
                if(destinationOperand[i] == '[')
                    break;
                beforeBracketsDest[i] = destinationOperand[i];
            }

            operandBinaryDest = operandHandling(beforeBracketsDest, symbolTableHead, destinationAddressingMethod, 0, 0, &hasLabel);
            free(beforeBracketsDest);

            isConstant = 0;
            if(!isNumber(inBracketsDest))
                isConstant = 1;

            indexBinaryDest = operandHandling(inBracketsDest, symbolTableHead, -1, isConstant, 0, &hasLabel);
            free(inBracketsDest);

            totalLength = strlen("0000") + strlen(opcode) + strlen(sourceAddressing) + strlen(destinationAddressing) + strlen("00\n") + strlen(operandBinarySrc) + (3*strlen("\n")) + strlen(indexBinarySrc) + strlen(operandBinaryDest) + strlen(indexBinaryDest) + 1;
            result = (char *)calloc(totalLength, sizeof(char));
            if(result == NULL){
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);
                exit(1);
            }

            if(lineNumber == 1)
                strcat(result, "0000");
            else
            {
                totalLength++;
                result = (char *)realloc(result, totalLength);
                if(result == NULL){
                    printIntError(ERROR_CODE_10);
                    for(i = 0; i < wordAmount; i++)
                        free(parsedLine[i]);
                    free(parsedLine);
                    exit(1);
                }
                strcat(result, "\n0000");
            }
            strcat(result, opcode);
            strcat(result, sourceAddressing);
            strcat(result, destinationAddressing);
            strcat(result, "00\n");
            strcat(result, operandBinarySrc);
            strcat(result, "\n");
            strcat(result, indexBinarySrc);
            strcat(result, "\n");
            strcat(result, operandBinaryDest);
            strcat(result, "\n");
            strcat(result, indexBinaryDest);

            free(operandBinarySrc);
            free(indexBinarySrc);
            free(operandBinaryDest);
            free(indexBinaryDest);
            free(sourceOperand);
            free(destinationOperand);

            lineNumber += 5;
            IC += (lineNumber - originalLineNumber);

            /* Write the line numbers and the index of the labels to the file */
            addLine(&linesDataArray, 5, 2, 3, &linesDataArraySize);

            return result;
        }
        else if(sourceAddressingMethod == 2)
        {
            /* get the index from the operand <label>[<index>] */
            inBrackets = (char *)calloc(strlen(sourceOperand), sizeof(char));
            if(inBrackets == NULL){
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);
                exit(1);
            }

            for(i = 0; (unsigned)i < strlen(sourceOperand); i++)
                if(sourceOperand[i] == '[')
                    break;
            i++;
            for(; (unsigned)i < strlen(sourceOperand); i++)
            {
                if(sourceOperand[i] == ']')
                    break;
                inBrackets[j] = sourceOperand[i];
                j++;
            }

            /* get the label from the operand <label>[<index>] */
            beforeBrackets = (char *)calloc(strlen(sourceOperand), sizeof(char));
            if(beforeBrackets == NULL){
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);
                exit(1);
            }

            for(i = 0; (unsigned)i < strlen(sourceOperand); i++)
            {
                if(sourceOperand[i] == '[')
                    break;
                beforeBrackets[i] = sourceOperand[i];
            }

            operandBinarySrc = operandHandling(beforeBrackets, symbolTableHead, sourceAddressingMethod, 0, 1, &hasLabel);
            free(beforeBrackets);

            isConstant = 0;
            if(!isNumber(inBrackets))
                isConstant = 1;

            indexBinarySrc = operandHandling(inBrackets, symbolTableHead, -1, isConstant, 1, &hasLabel);
            free(inBrackets);

            isConstant = 0;
            variable = (char *)calloc(strlen(sourceOperand), sizeof(char));
            for(i = 1; (unsigned)i < strlen(destinationOperand); i++)
                variable[i-1] = destinationOperand[i];
            if(destinationAddressingMethod == 0 && !isNumber(variable))
                isConstant = 1;

            free(variable);

            operandBinaryDest = operandHandling(destinationOperand, symbolTableHead, destinationAddressingMethod, isConstant, 0, &hasLabel);
            free(destinationOperand);
            
            totalLength = strlen("0000") + strlen(opcode) + strlen(sourceAddressing) + strlen(destinationAddressing) + strlen("00\n") + strlen(operandBinarySrc) + strlen("\n") + strlen(indexBinarySrc) + strlen("\n") + strlen(operandBinaryDest) + 1;
            result = (char *)calloc(totalLength, sizeof(char));
            if(result == NULL){
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);
                exit(1);
            }

            if(lineNumber == 1)
                strcat(result, "0000");
            else
            {
                totalLength++;
                result = (char *)realloc(result, totalLength);
                if(result == NULL){
                    printIntError(ERROR_CODE_10);
                    for(i = 0; i < wordAmount; i++)
                        free(parsedLine[i]);
                    free(parsedLine);
                    exit(1);
                }
                strcat(result, "\n0000");
            }
            strcat(result, opcode);
            strcat(result, sourceAddressing);
            strcat(result, destinationAddressing);
            strcat(result, "00\n");
            strcat(result, operandBinarySrc);
            strcat(result, "\n");
            strcat(result, indexBinarySrc);
            strcat(result, "\n");
            strcat(result, operandBinaryDest);

            free(operandBinarySrc);
            free(indexBinarySrc);
            free(operandBinaryDest);


            lineNumber += 4;
            IC += (lineNumber - originalLineNumber);

            addLine(&linesDataArray, 4, 2, -1, &linesDataArraySize);

            return result;
        }
        else if(destinationAddressingMethod == 2)
        {
            /* Translate the source operand */
            isConstant = 0;
            variable = (char *)calloc(strlen(sourceOperand), sizeof(char));
            for(i = 1; (unsigned)i < strlen(sourceOperand); i++)
                variable[i-1] = sourceOperand[i];
            if(sourceAddressingMethod == 0 && !isNumber(variable))
                isConstant = 1;

            free(variable);

            operandBinarySrc = operandHandling(sourceOperand, symbolTableHead, sourceAddressingMethod, isConstant, 1, &hasLabel);

            /* get the index from the operand <label>[<index>] */
            inBrackets = (char *)calloc(strlen(destinationOperand), sizeof(char));
            if(inBrackets == NULL){
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);
                exit(1);
            }

            for(i = 0; (unsigned)i < strlen(destinationOperand); i++)
                if(destinationOperand[i] == '[')
                    break;
            i++;
            for(; (unsigned)i < strlen(destinationOperand); i++)
            {
                if(destinationOperand[i] == ']')
                    break;
                inBrackets[j] = destinationOperand[i];
                j++;
            }

            /* get the label from the operand <label>[<index>] */
            beforeBrackets = (char *)calloc(strlen(destinationOperand), sizeof(char));
            if(beforeBrackets == NULL){
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);
                exit(1);
            }

            for(i = 0; (unsigned)i < strlen(destinationOperand); i++)
            {
                if(destinationOperand[i] == '[')
                    break;
                beforeBrackets[i] = destinationOperand[i];
            }

            isConstant = 0;
            if(!isNumber(inBrackets))
                isConstant = 1;

            indexBinaryDest = operandHandling(inBrackets, symbolTableHead, -1, isConstant, 1, &hasLabel);
            free(inBrackets);
            operandBinaryDest = operandHandling(beforeBrackets, symbolTableHead, destinationAddressingMethod, 0, 0, &hasLabel);
            free(beforeBrackets);

            totalLength = strlen("0000") + strlen(opcode) + strlen(sourceAddressing) + strlen(destinationAddressing) + strlen("00\n") + strlen(operandBinarySrc) + strlen("\n") + strlen(operandBinaryDest) + strlen("\n") + strlen(indexBinaryDest) + 1;
            result = (char *)calloc(totalLength, sizeof(char));
            if(result == NULL){
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);
                exit(1);
            }

            if(lineNumber == 1)
                strcat(result, "0000");
            else
            {
                totalLength++;
                result = (char *)realloc(result, totalLength);
                if(result == NULL){
                    printIntError(ERROR_CODE_10);
                    for(i = 0; i < wordAmount; i++)
                        free(parsedLine[i]);
                    free(parsedLine);
                    exit(1);
                }
                strcat(result, "\n0000");
            }
            strcat(result, opcode);
            strcat(result, sourceAddressing);
            strcat(result, destinationAddressing);
            strcat(result, "00\n");
            strcat(result, operandBinarySrc);
            strcat(result, "\n");
            strcat(result, operandBinaryDest);
            strcat(result, "\n");
            strcat(result, indexBinaryDest);

            free(operandBinarySrc);
            free(operandBinaryDest);
            free(indexBinaryDest);
            free(sourceOperand);
            free(destinationOperand);
            
            lineNumber += 4;
            IC += (lineNumber - originalLineNumber);

            addLine(&linesDataArray, 4, 3, -1, &linesDataArraySize);

            return result;
        }

        /* Translate the source operand */
        isConstant = 0;
        variable = (char *)calloc(strlen(sourceOperand), sizeof(char));
        for(i = 1; (unsigned)i < strlen(sourceOperand); i++)
            variable[i-1] = sourceOperand[i];
        if(sourceAddressingMethod == 0 && !isNumber(variable))
            isConstant = 1;

        free(variable);

        srcHasLabel = 0;
        operandBinarySrc = operandHandling(sourceOperand, symbolTableHead, sourceAddressingMethod, isConstant, 1, &srcHasLabel);

        /* Translate the destination operand */
        isConstant = 0;
        variable = (char *)calloc(strlen(destinationOperand), sizeof(char));
        for(i = 1; (unsigned)i < strlen(destinationOperand); i++)
            variable[i-1] = destinationOperand[i];
        if(destinationAddressingMethod == 0 && !isNumber(variable))
            isConstant = 1;

        free(variable);

        destHasLabel = 0;
        operandBinaryDest = operandHandling(destinationOperand, symbolTableHead, destinationAddressingMethod, isConstant, 0, &destHasLabel);

        totalLength = strlen("0000") + strlen(opcode) + strlen(sourceAddressing) + strlen(destinationAddressing) + strlen("00\n") + strlen(operandBinarySrc) + strlen("\n") + strlen(operandBinaryDest) + 1;
        result = (char *)calloc(totalLength, sizeof(char));
        if(result == NULL){
            printIntError(ERROR_CODE_10);
            for(i = 0; i < wordAmount; i++)
                free(parsedLine[i]);
            free(parsedLine);
            exit(1);
        }

        if(lineNumber == 1)
            strcat(result, "0000");
        else
        {
            totalLength++;
            result = (char *)realloc(result, totalLength);
            if(result == NULL){
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);
                exit(1);
            }
            strcat(result, "\n0000");
        }
        strcat(result, opcode);
        strcat(result, sourceAddressing);
        strcat(result, destinationAddressing);
        strcat(result, "00\n");
        strcat(result, operandBinarySrc);
        strcat(result, "\n");
        strcat(result, operandBinaryDest);
        
        /* In this free we got an invalid free error */
        free(operandBinarySrc);
        free(operandBinaryDest);
        free(sourceOperand);
        free(destinationOperand);

        lineNumber += 3;
        IC += (lineNumber - originalLineNumber);

        if(srcHasLabel && destHasLabel){
            addLine(&linesDataArray, 3, 2, 3, &linesDataArraySize);
        }
        else if(srcHasLabel){
            addLine(&linesDataArray, 3, 2, -1, &linesDataArraySize);
        }
        else if(destHasLabel){
            addLine(&linesDataArray, 3, 3, -1, &linesDataArraySize);
        }
        else
        {
            addLine(&linesDataArray, 3, -1, -1, &linesDataArraySize);
        }

        return result;
    }
    emptyReturn = (char *)calloc(6, sizeof(char));
    if (emptyReturn == NULL){
        printIntError(ERROR_CODE_10);
        for(i = 0; i < wordAmount; i++)
            free(parsedLine[i]);
        free(parsedLine);
        exit(1);
    }
    strcpy(emptyReturn, "ERROR\0");
    return emptyReturn;
}

/* Purpose is explained in the header file */
char* handleString(char **parsedLine, char *line, int wordAmount){
    int amountOfChars = 0;
    int i = 0;
    int lengthOfBinaryLine = 0;
    char *binaryNumber, *binaryLine, *emptyReturn;
    int hasClosingQuotation = 0;
    /* Count the amount of chars in the string not including the "" */
    for (i = 1; (unsigned)i < strlen(line) - 1; i++)
        amountOfChars++;

    lengthOfBinaryLine = (amountOfChars+1) * 15; /* Calculate the length of the binary line */

    binaryLine = (char *)calloc(lengthOfBinaryLine+1, sizeof(char)); /* Create a new string to store the binary line */
    if(binaryLine == NULL){
        printIntError(ERROR_CODE_10);
        for(i = 0; i < wordAmount; i++)
            free(parsedLine[i]);
        free(parsedLine);
        exit(1);
    }

    /* 
    Loop through the string
    The first char is " and the last char is ", the loop should run from 1 to amountOfChars 
    */
    for (i = 1; i <= amountOfChars + 1; i++)
    {
        /* Convert the character to ascii and then to binary */
        int number = line[i];
        /* Convert the number to a binary string */
        char *binaryNumber = intToBinary(number, BITS_AMOUNT);

        if(line[i] == '"'){
            hasClosingQuotation = 1;
            free(binaryNumber);
            break;
        }

        /* Add the binary number to the binary line */
        if(lineNumber == 1)
            strcat(binaryLine, binaryNumber);
        else {
            strcat(binaryLine, "\n");
            strcat(binaryLine, binaryNumber);
        }

        free(binaryNumber);
        lineNumber++;
    }
    if(hasClosingQuotation == 0)
    {
        printExtError(ERROR_CODE_37, fileLoc);
        noErrors = 0;
        emptyReturn = (char *)calloc(1, sizeof(char));
        if(emptyReturn == NULL){
            printIntError(ERROR_CODE_10);
            for(i = 0; i < wordAmount; i++)
                free(parsedLine[i]);
            free(parsedLine);
            exit(1);
        }
        return emptyReturn;
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
    DC += amountOfChars;
    lineNumber++;

    /* Write the line numbers to the file */
    addLine(&linesDataArray, amountOfChars, -1, -1, &linesDataArraySize);

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
    char *emptyReturn;
    char *binaryLine = (char *)calloc(15, sizeof(char)); /* Create a new string to store the binary line */
    if(binaryLine == NULL){
        printIntError(ERROR_CODE_10);
        for(i = 0; i < wordAmount; i++)
            free(parsedLine[i]);
        free(parsedLine);
        exit(1);
    }

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
            if(binaryLine == NULL){
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);
                exit(1);
            }
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
            variable = (char *)calloc(strlen(parsedLine[i])+1, sizeof(char));
            if(variable == NULL){
                printIntError(ERROR_CODE_10);
                for(i = 0; i < wordAmount; i++)
                    free(parsedLine[i]);
                free(parsedLine);
                exit(1);
            }
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
                free(variable);
            }
            else
            {
                free(variable);
                free(binaryLine);
                printExtError(ERROR_CODE_46, fileLoc);
                noErrors = 0;
                emptyReturn = (char *)calloc(1, sizeof(char));
                if(emptyReturn == NULL){
                    printIntError(ERROR_CODE_10);
                    for(i = 0; i < wordAmount; i++)
                        free(parsedLine[i]);
                    free(parsedLine);
                    exit(1);
                }
                return emptyReturn;
            }
        }
    }
    lineNumber += wordAmount - 1; /* Increment the line number */

    addLine(&linesDataArray, wordAmount - 1, -1, -1, &linesDataArraySize);

    /* Add a null terminator to the binary line */
    /* Return the binary line */
    return binaryLine;
}

/* Purpose is explained in the header file */
void handleLabel(char *label, Node **symbolTableHead, int type)
{
    /* Check if the label's name is valid */
    int i = 0;
    if(!isalpha(label[0]))
    {
        printExtError(ERROR_CODE_30, fileLoc);
        noErrors = 0;
        return;
    }
    for(i = 1; (unsigned)i < strlen(label); i++)
    {
        if(!isalpha(label[i]) && !isdigit(label[i]))
        {
            printExtError(ERROR_CODE_30, fileLoc);
            noErrors = 0;
            return;
        }
    }
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
    char *variable = (char *)calloc(strlen(parsedLine[1])+1, sizeof(char));
    if(variable == NULL){
        printIntError(ERROR_CODE_10);
        exit(1);
    }
    if(wordAmount == 3)
    {
        printExtError(ERROR_CODE_49, fileLoc);
        noErrors = 0;
        free(variable);
        return;
    }
    else if(wordAmount != 4)
    {
        printExtError(ERROR_CODE_50, fileLoc);
        noErrors = 0;
        free(variable);
        return;
    }
    /* Check if the constant is a number */
    if (isNumber(parsedLine[3]) == 0)
    {
        printExtError(ERROR_CODE_51, fileLoc);
        noErrors = 0;
        free(variable);
        return;
    }
    
    /* Make sure the constant is not already in the symbol table */
    found = 0;
    strcpy(variable, parsedLine[1]);
    searchNodeInList(*symbolTableHead, variable, &found);
    if (found == 1)
    {
        printExtError(ERROR_CODE_48, fileLoc);
        noErrors = 0;
        free(variable);
        return;
    }
    /* Add the constant to the symbol table */
    else
        addNode(symbolTableHead, parsedLine[1], "mdefine", atoi(parsedLine[3]));

    free(variable);
}

void executeFirstPass(char *file, char **outputFileName)
{
    int i;
    char line[MAX_LINE_LENGTH + 1];
    char *cleanedLine, *noCommas;
    char *binaryLine;
    FILE *outputFile;
    Node *current;
    FILE *inputFile = fopen(file, "r");
    if (inputFile == NULL)
    {
        printIntError(ERROR_CODE_14);
        exit(1);
    }
    /* output file is a temporary file that will be deleted after the second pass
    the outputFile's name is "tempFile.ob" */
    *outputFileName = (char *)calloc(14, sizeof(char));
    if (*outputFileName == NULL)
    {
        printIntError(ERROR_CODE_10);
        exit(1);
    }

    strcpy(*outputFileName, "tempFile.ob");

    linesDataArray = (lineData *)calloc(1, sizeof(lineData));

    outputFile = fopen(*outputFileName, "w");
    if (outputFile == NULL)
    {
        printIntError(ERROR_CODE_13);
        free(*outputFileName); 
        exit(1);
    }
    
    fileLoc.fileName = file;    
    fileLoc.line = 1;

    while(fgets(line, MAX_LINE_LENGTH, inputFile) != NULL) 
    {
        if(!strstr(line, ".string"))
        {
            noCommas = removeCommas(line);
            cleanedLine = cleanLine(noCommas);
            free(noCommas);
        }
        else 
            cleanedLine = cleanLine(line);
        binaryLine = checkLineType(cleanedLine, line);

        if (strcmp(binaryLine, "ERROR") != 0 && strcmp(binaryLine, "CONSTANT") != 0)
        {
            fprintf(outputFile, "%s", binaryLine);
        }

        free(binaryLine);
        free(cleanedLine);

        lineNumberSrcFile++;
        fileLoc.line++;
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

    /* Print the linesDataArray */
    for(i = 0; i < linesDataArraySize; i++)
    {
        printf("line: %d, src: %d, dest: %d\n", linesDataArray[i].binaryLinesWritten, linesDataArray[i].firstLabelIndex, linesDataArray[i].secondLabelIndex);
    }

    if(noErrors)
        executeSecondPass(file, *outputFileName, symbolTable, IC, DC, linesDataArray);

    /* Loop through the symbol table and free the nodes */
    while (symbolTable != NULL)
    {
        current = symbolTable;
        symbolTable = symbolTable->next;
        free(current->name);
        free(current->data);
        free(current);
    }
    /* Free the linesDataArray */
    free(linesDataArray);
}



