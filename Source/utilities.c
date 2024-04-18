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
    char *cleanedLine = (char *)calloc(strlen(line) + 2, sizeof(char));
    if (cleanedLine == NULL)
    {
        printIntError(ERROR_CODE_10);
    }
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
    
    /* If the first character is a space, shift everything to the left */
    while(cleanedLine[0] == ' ')
    {
        for(int i = 0; i < cleanedIndex; i++)
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
char **parseLine(char *line, int wordAmount)
{
    /* Parse the line by spaces, commas, tabs */
    char **parsedLine = (char **)calloc(wordAmount+1, sizeof(char *));
    if (parsedLine == NULL)
    {
        printIntError(ERROR_CODE_10);
    }
    int wordIndex = 0; /* Index for the word */
    int lineIndex = 0; /* Index for the line */
    int wordLength = 0; /* Length of the word */

    /* Loop through the line */
    while (line[lineIndex] != '\0')
    {
        /* If the current character is a space, comma or tab, skip it */
        if (line[lineIndex] == ' ' || line[lineIndex] == ',' || line[lineIndex] == '\t')
        {
            lineIndex++;
            continue;
        }

        /* Count the length of the word */
        while (line[lineIndex] != ' ' && line[lineIndex] != ',' && line[lineIndex] != '\t' && line[lineIndex] != '\0')
        {
            wordLength++;
            lineIndex++;
        }

        /* Allocate memory for the word */
        parsedLine[wordIndex] = (char *)calloc(wordLength + 1, sizeof(char));
        if (parsedLine[wordIndex] == NULL)
        {
            printIntError(ERROR_CODE_10);
        }

        /* Copy the word to the parsed line */
        for (int i = 0; i < wordLength; i++)
        {
            parsedLine[wordIndex][i] = line[lineIndex - wordLength + i];
        }

        /* Add the null character at the end of the word */
        parsedLine[wordIndex][wordLength] = '\0';

        /* Increment the indexes */
        wordIndex++;
        wordLength = 0;
    }

    /* Return the parsed line */
    return parsedLine;
}

/* Explained in the header file */
int isNumber(char *str)
{
    /* Loop through the string, allow for - or + at the beginning */
    for(int i = 0; i < strlen(str)-1; i++)
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
    /* Initialize the count to 0 */
    int count = 0;
    int i = 0;
    /* Loop through the line */
    while(line[i] != '\0')
    {
        /* If the current char is a char and the next char is a space, increment the count */
        if(line[i] != ' ' && line[i+1] == ' ')
            count++;
        i++;
    }
    /* Return the count (add 1 because the last word doesn't have a space after it) */
    return count+1;
}

/* Explained in the header file */
char *intToBinary(int num, int bits)
{
    /* Bits is the number of bits to represent the number */
    char *binary = (char *)calloc(bits+1, sizeof(char));
    if (binary == NULL)
        printIntError(ERROR_CODE_10);

    /* Loop through the bits */
    for(int i = bits-1; i >= 0; i--)
    {
        /* If the number is odd, add 1 to the binary string */
        binary[i] = (num % 2 == 1) ? '1' : '0';
        num /= 2;
    }
    /* Add the null character at the end of the string */
    binary[bits] = '\0';
    /* Return the binary string */
    return binary;
}

/* Explained in the header file */
char *getAddressingMethod(char *operand, Node *symbolTable, int *addressingMethod)
{
    /* If immediate addressing method */
    if(operand[0] == '#')
    {
        *addressingMethod = 0;
        return "00";
    }

    int openingBracket = -1;
    int closingBracket = -1;
    /* If index addressing method */
    for(int i = 0; i < strlen(operand); i++)
    {
        if(operand[i] == '[')
            openingBracket = i;
        else if(operand[i] == ']')
            closingBracket = i;
    }
    if(openingBracket != -1 && closingBracket != -1 && closingBracket > openingBracket)
    {
        /* Parse the operand from the brackets */
        char inBrackets[closingBracket-openingBracket];
        for(int i = openingBracket+1; i < closingBracket; i++)
        {
            inBrackets[i-openingBracket-1] = operand[i];
        }
        inBrackets[closingBracket-openingBracket-1] = '\0';

        int isIndexLegal = -1;
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
    /* Allocate memory for the cleaned line - same size + 1 for the null character */
    char *cleanedLine = (char *)calloc(strlen(line) + 1, sizeof(char));
    if (cleanedLine == NULL)
    {
        printIntError(ERROR_CODE_10);
    }
    int originalIndex = 0; /* Index for the original line */
    int cleanedIndex = 0; /* Index for the cleaned line */

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