#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/preAssembler.h"
#include "../Headers/errors.h"
#include "../Headers/globalVariables.h"

char *cleanLine(char *line)
{
    char *cleanedLine = (char *)malloc(sizeof(char) * (strlen(line)+1));
    int i = 0;
    int j = 0;
    while (line[i] != '\0')
    {
        if(line[i] == '\t')
        {
            line[i] = ' ';
        }
        if(line[i] == ' ' && (line[i+1] == ' ' || line[i+1] == '\t' || line[i+1] == '\n' || line[i+1] == '\0'))
        {
            i++;
            continue;
        }
        if(line[i] == ' ')
        {
            if(j!=0 && cleanedLine[j-1] != ' ')
            {
                cleanedLine[j] = line[i];
                j++;
            }
        }
        if (line[i] != ' ' && line[i] != '\t' && line[i] != '\n')
        {
            cleanedLine[j] = line[i];
            j++;
        }
        i++;
    }
    cleanedLine[j] = ' ';
    cleanedLine[j+1] = '\0';
    return cleanedLine;
}

// Propably will be removed - isn't really useful
int copyLineToFile(char *fileName, char *line)
{
    FILE *file = fopen(fileName, "a");

    if (file == NULL)
    {
        return 0;
    }

    fprintf(file, "%s\n", line);
    fclose(file);

    return 1;
}