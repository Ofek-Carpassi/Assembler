#include "../Headers/utilities.h" // Include the header file with the function declarations
#include <stdlib.h> // Used for malloc
#include <string.h> // Used for strlen
#include <ctype.h> // Used for isdigit

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

int findCount(int n)
{
    int count = 0;
 
    // Remove last digit from number
    // till number is 0
    while (n != 0) {
 
        // Increment count
        count++;
        n /= 10;
    }
 
    // return the count of digit
    return count;
}

int isNumber(const char *str)
{
    if (*str == '-') { // handle negative numbers
        str++;
    }
    while (*str) {
        if (!isdigit(*str)) {
            return 0;
        }
        str++;
    }
    return 1;
}

int countWords(char *line)
{
    int count = 0;
    int i = 0;
    while(line[i] != '\0')
    {
        if(line[i] == ' ')
        {
            count++;
        }
        i++;
    }
    return count;
}

void intToBinary(int num, char **res) 
{
    int bits = sizeof(num) * 8; // number of bits in an integer
    *res = (char *)malloc(bits + 1); // +1 for the null terminator
    if (*res == NULL) {
        // Handle error
        return;
    }

    for (int i = 0; i < bits; i++) 
    {
        (*res)[bits - 1 - i] = (num & (1 << i)) ? '1' : '0';
    }
    (*res)[bits] = '\0'; // null terminator
}