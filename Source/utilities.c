#include "../Headers/utilities.h" // Include the header file with the function declarations
#include <stdlib.h> // Used for malloc
#include <string.h> // Used for strlen

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