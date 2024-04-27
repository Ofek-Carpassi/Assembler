#include <stdlib.h> 
#include <string.h> 
#include <ctype.h> 
#include "../Headers/globalVariables.h"
#include "../Headers/errors.h"
#include "../Headers/dataStructures.h"
#include "../Headers/utilities.h"

void freeStringArray(char **array, int size)
{
    int i = 0;
    for(i = 0; i < size; i++)
        free(array[i]);
    free(array);
}

/* Explained in the header file */
char *cleanLine(char *line)
{
    /* Initialize all needed variables */
    int originalIndex = 0, cleanedIndex = 0, i = 0;
    char *cleanedLine;
    /* Allocate memory for the cleaned line - same size + 1 for the null character */
    cleanedLine = (char *)calloc(strlen(line) + 2, sizeof(char));
    if (cleanedLine == NULL)
    {
        printIntError(ERROR_CODE_10);
        exit(1);
    }

    /* Loop through the original line */
    while (line[originalIndex] != '\0')
    {
        /* Replace tabs with spaces - this way we don't need to check for tabs later (in the assembler) */
        if(line[originalIndex] == '\t')
        {
            line[originalIndex] = ' ';
        }

        /* If the current character is a whitespace and the next character is a whitespace, skip it (we want to keep one space) */
        if(line[originalIndex] == ' ' && (line[originalIndex+1] == ' ' || line[originalIndex+1] == '\t' || line[originalIndex+1] == '\n' || line[originalIndex+1] == '\0'))
        {
            originalIndex++;
            continue;
        }

        /* Copy the character to the cleaned line */
        cleanedLine[cleanedIndex] = line[originalIndex];

        /* Increment the indexes */
        cleanedIndex++; 
        originalIndex++;
    }
    
    /* If the first character is a space, shift everything to the left */
    while(cleanedLine[0] == ' ')
    {
        for(i = 0; i < cleanedIndex; i++)
        {
            cleanedLine[i] = cleanedLine[i+1];
        }
        cleanedIndex--;
    }

    /* Add the null character and the space at the end of the line */
    cleanedLine[cleanedIndex] = '\0';  

    /* Return the cleaned line */
    return cleanedLine;
}

/* Explained in the header file */
char** parseLine(char* input, int numWords, location* loc, int* noErrors) {
    char** result = NULL;
    char* token;
    int i = 0, wordCount = 0;

    if (input == NULL) {
        printIntError(45);
        *noErrors = 0;
        return NULL;
    }

    result = (char**)calloc(numWords, sizeof(char*));
    if (result == NULL) {
        printIntError(10);
        *noErrors = 0;
        return NULL;
    }

    if (strstr(input, ".string")) {
        result[0] = (char*)calloc(8, sizeof(char)); 
        if (result[0] == NULL) {
            printIntError(10);
            *noErrors = 0;
            free(result);
            return NULL;
        }
        strcpy(result[0], ".string");

        result[1] = (char*)calloc(strlen(input) - 7, sizeof(char)); 
        if (result[1] == NULL) {
            printIntError(10);
            *noErrors = 0;
            free(result[0]);
            free(result);
            return NULL;
        }
        strcpy(result[1], input + 8); 
    } else {
        token = strtok(input, " ,\t");

        while (token != NULL && wordCount < numWords) {
            result[wordCount] = (char*)calloc(strlen(token) + 1, sizeof(char));
            if (result[wordCount] == NULL) {
                printIntError(10);
                *noErrors = 0;
                for (i = 0; i < wordCount; i++) {
                    free(result[i]);
                }
                free(result);
                return NULL;
            }
            strcpy(result[wordCount], token);
            token = strtok(NULL, " ,\t");
            wordCount++;
        }

        if (wordCount != numWords) {
            printIntError(45);
            *noErrors = 0;
            for (i = 0; i < wordCount; i++) {
                free(result[i]);
            }
            free(result);
            return NULL;
        }
    }

    return result;
}

/* Explained in the header file */
int isNumber(char *str)
{
    int i = 0;
	if(str[strlen(str)-1] == '\n')
		str[strlen(str)-1] = '\0';

    /* Loop through the string, allow for - or + at the beginning */
    for(i = 0; i < strlen(str)-1; i++)
    {
        if(i == 0 && (str[i] == '-' || str[i] == '+'))
            continue;
        /* If the current character is not a digit, return 0 */
        if(!isdigit(str[i]))
            return 0;
    }
    /* Return 1 if the string is a number */
    return 1;
}

/* Explained in the header file */
int countWords(char *line)
{
    int count = 0;
    int i = 0;
    int inString = 0;  

    while(line[i] != '\0')
    {
        if(line[i] == '"') {
            inString = !inString;  
        } else if(!inString && line[i] != ' ' && (line[i+1] == ' ' || line[i+1] == '\0')) {
            count++;
        }
        i++;
    }

    return count;
}

/* Explained in the header file */
char *intToBinary(int num, int bits)
{
    int i = 0;
    char *binary = (char *)calloc(bits + 1, sizeof(char));
    if(binary == NULL)
    {
        printIntError(ERROR_CODE_10);
        exit(1);
    }

    if(num < 0)
    {
        num = -num; 
        num = num - 1;
        for(i = bits-1; i >= 0; i--)
        {
            binary[i] = (num % 2 == 0) ? '1' : '0';
            num /= 2;
        }
    }
    else
    {
        for(i = bits-1; i >= 0; i--)
        {
            binary[i] = (num % 2 == 0) ? '0' : '1';
            num /= 2;
        }
    }

    binary[bits] = '\0';

    return binary;
}

/* Explained in the header file */
char *getAddressingMethod(char *operand, Node *symbolTable, int *addressingMethod)
{
    int openingBracket = -1, closingBracket = -1, i = 0, isIndexLegal = -1;
    char *inBrackets;
    /* If immediate addressing method */
    if(operand[0] == '#')
    {
        *addressingMethod = 0;
        return "00";
    }

    /* If index addressing method */
    for(i = 0; i < strlen(operand); i++)
    {
        if(operand[i] == '[')
            openingBracket = i;
        else if(operand[i] == ']')
            closingBracket = i;
    }
    if(openingBracket != -1 && closingBracket != -1 && closingBracket > openingBracket)
    {
        /* Parse the operand from the brackets */
        inBrackets = (char *)calloc(closingBracket-openingBracket+1, sizeof(char));
        if(inBrackets == NULL)
        {
            printIntError(ERROR_CODE_10);
            exit(1);
        }
        for(i = openingBracket+1; i < closingBracket; i++)
        {
            inBrackets[i-openingBracket-1] = operand[i];
        }
        inBrackets[closingBracket-openingBracket-1] = '\0';

        isIndexLegal = -1;
        /* Check if the value inside the brackets is a number or a constant */
        if(isNumber(inBrackets))
            isIndexLegal = 1;
        else
        {
            int found = 0;
            Node *current = searchNodeInList(symbolTable, inBrackets, &found);
            if(found)
                if(strcmp(current->data, "mdefine") == 0)
                    isIndexLegal = 1;
        }
        if(isIndexLegal == -1)
        {
            printIntError(ERROR_CODE_13);
            return "ERROR";
        }

        /* Make sure the value before the brackets is a label defined with .data or .string */
        *addressingMethod = 2;

        /* No need to check if the label is defined because label translation is in second pass */
        free(inBrackets);
        return "10";
    }

    /* If register addressing method */
    if(operand[0] == 'r')
    {
        *addressingMethod = 3;
        return "11";
    }

    /* direct addressing method is when the operand is a label - label translation is in second pass - so we don't need to check it here */
    *addressingMethod = 1;
    return "01";
}

char *removeCommas(char *line)
{
    int originalIndex = 0; /* Index for the original line */
    int cleanedIndex = 0; /* Index for the cleaned line */
    char *cleanedLine; /* The cleaned line */
    /* Allocate memory for the cleaned line - same size + 1 for the null character */
    cleanedLine = (char *)calloc(strlen(line) + 1, sizeof(char));
    if (cleanedLine == NULL)
    {
        printIntError(ERROR_CODE_10);
        exit(1);
    }

    /* Loop through the original line */
    while (line[originalIndex] != '\0')
    {
        /* If the current character is a comma, skip it */
        if(line[originalIndex] == ',')
        {
            /* Remove the comma but if the next character isn't a space, add a space */
            if(line[originalIndex+1] != ' ')
            {
                cleanedLine[cleanedIndex] = ' ';
                cleanedIndex++;
            }
            originalIndex++;
            continue;
        }

        /* Copy the character to the cleaned line */
        cleanedLine[cleanedIndex] = line[originalIndex];

        /* Increment the indexes */
        cleanedIndex++; 
        originalIndex++;
    }

    /* Add the null character and the space at the end of the line */
    cleanedLine[cleanedIndex] = '\0';  

    /* Return the cleaned line */
    return cleanedLine;
}

void isLegalCommas(char *line, int *noErrors, location *loc, char **parsedLine, int wordAmount)
{
    int hasSeenComma = 0;
    int hasSeperatingComma = 0;
    int currentWord = 0;
    int firstWord = 0;
    int i = 0;
    int startingIndex = 0;
    if(strstr(line, ".define"))
    {
        if(strstr(line, ","))
        {
            printExtError(ERROR_CODE_27, *loc);
            *noErrors = 0;
        }
        return;
    }
    if(strstr(line, ".string"))
    {
        if(strstr(parsedLine[0], ","))
        {
            printExtError(ERROR_CODE_27, *loc);
            *noErrors = 0;
        }
        return;
    }
    if(parsedLine[0][strlen(parsedLine[0])-1] == ':')
    {
        /* Start from the second word - there could be spaces after the label - calculate i */
        i = strlen(parsedLine[0]);
        while(line[i] == ' ' || line[i] == '\t')
        {
            i++;
        }
        firstWord = 1;
        currentWord = 1;
        startingIndex = i;
    }
    /*printf("line: %s\n", line);*/
    for(; i < strlen(line); i++) {
        /*printf("line[%d]: %c\n", i, line[i]);*/
        
        if(line[i] == ',') {
            if(hasSeenComma) {
                printExtError(ERROR_CODE_27, *loc);
                *noErrors = 0;
            } else if(currentWord == firstWord) {
                printExtError(ERROR_CODE_28, *loc);
                *noErrors = 0;
            } else if(currentWord == wordAmount) {
                printExtError(ERROR_CODE_28, *loc);
                *noErrors = 0;
            }
            hasSeenComma = 1;
            hasSeperatingComma = 1;
        } else if(line[i] != ' ' && line[i] != ',' && i > 0 && (line[i-1] == ' ' || line[i-1] == ',')) {
            if(hasSeperatingComma == 0 && currentWord != firstWord) {
                printExtError(ERROR_CODE_22, *loc);
                *noErrors = 0;
            }
            if(i != startingIndex) {
                currentWord++;
            }
            hasSeenComma = 0;
            hasSeperatingComma = 0;
        } else {
            hasSeenComma = 0;
        }
    }
}
