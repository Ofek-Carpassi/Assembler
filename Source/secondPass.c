#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Headers/dataStructures.h"
#include "../Headers/utilities.h"
#include "../Headers/secondPass.h"

#define MAX_LINE_LENGTH 1024

int InstructionCounter = 0;
int objLine = 1;

/* the function isEntry check if the line has .entry */
int isEntry(char *line) {
    return (strstr(line, ".entry") != NULL);
}

int getWordCount(char *line) {
    if (strstr(line, ".data") != NULL || strstr(line, ".extern") != NULL) {
        /* We'll count the number of commas in the line and add 1 */
        int count = 0;
        int i = 0;
        for (i = 0; i < strlen(line); i++) {
            if (line[i] == ',') {
                count++;
            }
        }
        return count + 1;
    } else if (strstr(line, ".string") != NULL) {
        /* We'll count the number of chars in the "" */
        int count = 0;
        int i = 0;
        while (line[i] != '"') {
            i++;
        }
        i++;
        while (line[i] != '"') {
            count++;
            i++;
        }
        return count + 1;
    }
    return 0;
}

char *convertToEncryptedBase4(char *binaryLine) {
    int binaryLineLength = strlen(binaryLine);
    int base4LineLength = binaryLineLength / 2;

    char *encryptedLine = (char *)calloc(base4LineLength + 1, sizeof(char));

    int i, j;

    printf("Binary line: %s\n", binaryLine);
    for (i = 0, j = 0; i < binaryLineLength; i += 2, j++) {
        int value = 0;
        if (binaryLine[i] == '1') {
            value += 2;
        }
        if (i + 1 < binaryLineLength && binaryLine[i + 1] == '1') {
            value += 1;
        }

        switch (value) {
            case 0:
                encryptedLine[j] = '*';
                break;
            case 1:
                encryptedLine[j] = '#';
                break;
            case 2:
                encryptedLine[j] = '%';
                break;
            case 3:
                encryptedLine[j] = '!';
                break;
        }
    }

    encryptedLine[base4LineLength] = '\0'; 

    return encryptedLine;
}

int hasLabel(char *line) {
    return (strstr(line, ":") != NULL);
}

char *getLabelFromLine(char *line, int index) {
    /* Parse the line to get the label */
    char **parsedLine;
    char *label;
    int wordAmount = getWordCount(line);
    int i;
    parsedLine = parseLine(cleanLine(removeCommas(line)), wordAmount);
    label = parsedLine[index];
    if(parsedLine[0][strlen(parsedLine[0])-1] == ':') {
        label = parsedLine[index+1];
    }
    for(i = 0; i < strlen(label); i++) {
        if(label[i] == '[')
            label[i] = '\0';
    }
    printf("Label: %s\n", label);
    return label;
}

void executeSecondPass(char *srcFile, char *tmpFileName, Node *symbolTableHead) {
    char line[MAX_LINE_LENGTH + 1];
    char tmpLine[MAX_LINE_LENGTH + 1];
    char numbersLine[MAX_LINE_LENGTH + 1];
    int numbers[3];
    int i = 0, j = 0;
    char *base4 = NULL;
    FILE *obj = NULL;
    char *label = NULL;
    Node *current = NULL;
    int found = 0;
    char *token = NULL;

    FILE *file = fopen(srcFile, "r");
    FILE *tmpFile = fopen(tmpFileName, "r");
    FILE *lineNumbers = fopen("lineNumbers.txt", "r");
    
    char *outputFileName = (char *)calloc(strlen(srcFile) + 5, sizeof(char));
    strcpy(outputFileName, srcFile);
    /* Remove the .am extension */
    outputFileName[strlen(outputFileName) - 3] = '\0';
    strcat(outputFileName, ".ob");

    obj = fopen(outputFileName, "w");

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        printf("Line: %s", line);
        printf("isEntry: %d\n\n", isEntry(line));
        
        /* If the line doesn't have .entry - we'll do the if statement */
        if (isEntry(line) == 0 && !strstr(line, ".define")) {
            /* We have a file called lineNumbers.txt that gives us crazy information
            the first number is the amount of binary lines the matching srcLine writes
            the second number (if exists) is the index+1 of a label in the line
            the third number (if exists) is the index+1 of a label in the line..
            and so on
            We'll parse the lineNumbers file and get the first number
            If there are no more numbers, we'll just write the lines to the obj file
            If there are more numbers, we'll read n lines from the tmp file and write them to the obj file
            then we'll convert the label's data from the symbol table to binary and write it in the matching line
            */
            fgets(numbersLine, MAX_LINE_LENGTH, lineNumbers);
            printf("Numbers line: %s", numbersLine);
            /* parse the line to numbers - the max amount of numbers is 3 */
            token = strtok(numbersLine, " ");
            i = 0;
            while (token != NULL) {
                numbers[i] = atoi(token);
                token = strtok(NULL, " ");
                i++;
            }
            printf("i: %d\n", i);
            /* If there are no more numbers, we'll just write the lines to the obj file */
            if (i == 1) {
                for(j = 0; j < numbers[0] + 1; j++) {
                    fgets(tmpLine, MAX_LINE_LENGTH, tmpFile);
                    fprintf(obj, "%s", tmpLine);
                }
            } else {
                /* skip (numbers[0] - numbers[1]) lines */
                for(j = 0; j < numbers[0] - numbers[1] + 1; j++) {
                    fgets(tmpLine, MAX_LINE_LENGTH, tmpFile);
                    fprintf(obj, "%s", tmpLine);
                }
                /* Get the numbers[1]-1 word from the line */
                label = getLabelFromLine(line, numbers[1]-1);
                printf("Label: %s\n", label);
                if(label[strlen(label)-1] == '\n') {
                    label[strlen(label)-1] = '\0';
                }
                /* Get the label's data from the symbol table */
                current = searchNodeInList(symbolTableHead, label, &found);
                if (found) {
                    base4 = intToBinary(current->line, 14);
                    fprintf(obj, "%s\n", base4);
                }
                printf("Base4: %s\n", base4);
                printf("current->line: %d\n", current->line);
                free(base4);
                /* skip (numbers[2] - numbers[1]) if numbers[2] exists */
                if (i == 3) {
                    for(j = 0; j < numbers[2] - numbers[1] + 1; j++) {
                        fgets(tmpLine, MAX_LINE_LENGTH, tmpFile);
                        fprintf(obj, "%s", tmpLine);
                    }
                    label = getLabelFromLine(line, numbers[2]-1);
                    current = searchNodeInList(symbolTableHead, label, &found);
                    if (found) {
                        base4 = intToBinary(current->line, 14);
                        fprintf(obj, "%s\n", base4);
                    }
                }
            }
        }
    }

    fclose(file);
    fclose(obj);
    fclose(tmpFile);

    /*remove(tmpFileName);*/

    free(outputFileName);

    return;
}