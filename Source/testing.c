#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "../Headers/errors.h"
#include "../Headers/preAssembler.h"

int main()
{
    char *line = "    add        r1,         r2, r3";
    char *cleanedLine = cleanLine(line);
    printf("%s\n", cleanedLine);
    copyLineToFile("txt.txt", cleanedLine);
    // print the line from the file
    FILE *file = fopen("txt.txt", "r");
    if (file == NULL)
    {
        printf("Error: file not found\n");
        return 0;
    }
    char c;
    while ((c = fgetc(file)) != EOF)
    {
        printf("%c", c);
    }
    fclose(file);

    
    return 0;
}