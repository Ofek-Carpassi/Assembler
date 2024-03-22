#include <stdlib.h> // Used for malloc
#include <string.h> // Used for strlen
#include <ctype.h> // Used for isdigit
#include "../Headers/globalVariables.h" // Include the header file with the global variables
#include "../Headers/errors.h" // Include the header file with the error codes
#include "../Headers/dataStructers.h" // Include the header file with the data structures
#include "../Headers/utilities.h" // Include the header file with the function declarations

/* Explained in the header file */
char *cleanLine(char *line)
{
    /* Allocate memory for the cleaned line - same size + 1 for the null character */
    char *cleanedLine = (char *)malloc(sizeof(char) * (strlen(line)+1));
    int originalIndex = 0; /* Index for the original line */
    int cleanedIndex = 0; /* Index for the cleaned line */

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
    
    /* Add the null character and the space at the end of the line */
    cleanedLine[cleanedIndex] = ' ';
    cleanedLine[cleanedIndex+1] = '\0';

    /* If the first character is a space, shift everything to the left */
    if(cleanedLine[0] == ' ')
    {
        /* Shift everything one character to the left */
        for(int i = 0; i < strlen(cleanedLine); i++)
        {
            cleanedLine[i] = cleanedLine[i+1];
        }
    }

    if(cleanedLine[0] == ' ')
    {
        /* Shift everything one character to the left */
        for(int i = 0; i < strlen(cleanedLine); i++)
        {
            cleanedLine[i] = cleanedLine[i+1];
        }   
    }

    //printf("%s\n", cleanedLine);

    /* Return the cleaned line */
    return cleanedLine;
}

/* Explained in the header file */
void parseLine(char *line, char *parsedOutput[])
{
    // print the array
    int c = 0; /* Index for the parsed output */
    int startIndex = 0;
    int arrayIndex = 0;
    int lineIndex = 0;

    while((c = line[lineIndex]) != '\0' && c != '\n')
    {
        if(c == ' ')
        {
            if(lineIndex == 0)
            {
                continue;
            }
            else
            {
                parsedOutput[arrayIndex] = (char *)malloc(sizeof(char) * (lineIndex - startIndex + 1));
                for(int i = startIndex; i < lineIndex; i++)
                {
                    parsedOutput[arrayIndex][i-startIndex] = line[i];
                }
                parsedOutput[arrayIndex][lineIndex - startIndex] = '\0';
                arrayIndex++;
                startIndex = lineIndex + 1;
            }
        }
        lineIndex++;
    }
    parsedOutput[arrayIndex] = (char *)malloc(sizeof(char) * (lineIndex - startIndex + 1));
    for(int i = startIndex; i < lineIndex; i++)
    {
        parsedOutput[arrayIndex][i-startIndex] = line[i];
    }
    parsedOutput[arrayIndex][lineIndex - startIndex] = '\0';
    arrayIndex++;
}

/* Explained in the header file */
int isNumber(const char *str)
{
    if (*str == '-') { // handle negative numbers
        str++;
    }
    /* Loop through the string */
    while (*str) {
        /* If the current character is not a digit, return 0 */
        if (!isdigit(*str++)) {
            return 0;
        }
    }
    /* If all characters are digits, return 1 */
    return 1;
}

/* Explained in the header file */
int countWords(char *line)
{
    /* Initialize the count to 0 */
    int count = 0;
    int i = 0;
    /* Loop through the line */
    while(line[i] != '\0')
    {
        /* If the current character is a space, increment the count */
        if(line[i++] == ' ')
        {
            count++;
        }
    }
    /* Return the count (add 1 because the last word doesn't have a space after it) */
    return count+1;
}

/* Explained in the header file */
void intToBinary(int num, char **res) 
{
    /* loop through the bits of the number */
    for (int i = 0; i < BITS_AMOUNT; i++) 
    {
        /* if the bit is 1, set the corresponding bit in the result to 1, otherwise set it to 0 */
        (*res)[BITS_AMOUNT - 1 - i] = (num & (1 << i)) ? '1' : '0';
    }
    /* add the null terminator to the end of the result */
    (*res)[BITS_AMOUNT] = '\0';
}

/* Explained in the header file */
char *intToBinaryString(int num) 
{
    /* Allocate memory for the result */
    char *res = (char *)malloc(sizeof(char) * (BITS_AMOUNT + 1)); // +1 for the null terminator
    if (res == NULL)
    {
        printIntError(ERROR_CODE_10);
    }
    /* Convert the number to binary */
    intToBinary(num, &res);
    /* add a new line character to the end of the result */
    strcat(res, "\n");
    /* return the result */
    return res;
}

/* Explained in the header file */
char *addressingMethod(char *operand, Node *symbolTable)
{
    /* If the operand is a register, return the register addressing method */
    if(operand[0] == '#')
        return "00";
    else if(operand[0] == 'r')
    {
        if(strlen(operand) > 2)
        {
            printIntError(ERROR_CODE_32);
            return "11";
        }
        if(operand[1] < '0' || operand[1] > '7')
        {
            printIntError(ERROR_CODE_32);
            return "11";
        }
        return "11";
    }

    /* Check if constantIndex addressing or direct addressing */
    int found = 0;
    Node *node = searchNodeInList(symbolTable, operand, &found);
    if(found)
    {
        return "01";
    }
    else
    {
        return "10";
    }
}