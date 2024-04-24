#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Headers/dataStructures.h"
#include "../Headers/utilities.h"
#include "../Headers/secondPass.h"
#include "../Headers/Errors.h"
#include "../Headers/globalVariables.h"

int InstructionCounter = 0;
int objLineNumber = 1;

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
    parsedLine = parseLine(cleanLine(removeCommas(line)), wordAmount, NULL, NULL);
    label = parsedLine[index];
    if(parsedLine[0][strlen(parsedLine[0])-1] == ':') {
        label = parsedLine[index+1];
    }
    for(i = 0; i < strlen(label); i++) {
        if(label[i] == '[')
            label[i] = '\0';
    }
    return label;
}

void merge(EntryExtern *arr, int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    EntryExtern *L = (EntryExtern *)calloc(n1, sizeof(EntryExtern));
    EntryExtern *R = (EntryExtern *)calloc(n2, sizeof(EntryExtern));

    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if (L[i].value <= R[j].value) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }

    free(L);
    free(R);
}

void mergeSort(EntryExtern *arr, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;

        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);

        merge(arr, l, m, r);
    }
}

void executeSecondPass(char *srcFile, char *tmpFileName, Node *symbolTableHead, int firstPassIC, int firstPassDC, lineData *linesDataArray) {
    char line[MAX_LINE_LENGTH + 1], tmpLine[MAX_LINE_LENGTH + 1], numbersLine[MAX_LINE_LENGTH + 1];
    char *base4 = NULL;
    FILE *obj = NULL;
    char *label = NULL;
    Node *current = NULL;
    int found = 0;
    int numbersAmount = 0;
    char *binaryLine = NULL;
    char *binaryNumber = NULL;
    int entryFileCreated = 0;
    char *entryFileName = NULL;
    char *externFileName = NULL;
    FILE *entryFile = NULL;
    FILE *externFile = NULL;
    int entriesCount = 0;
    int externsCount = 0;
    int linesDataIndex = 0;
    int i = 0;
    EntryExtern *entriesArray = NULL;
    EntryExtern *externsArray = (EntryExtern *)calloc(1, sizeof(EntryExtern));
    FILE *file = fopen(srcFile, "r");
    FILE *tmpFile = fopen(tmpFileName, "r");
    FILE *lineNumbers = fopen("lineNumbers.txt", "r");
    int writtenBinaryLines = 0, firstLabelIndex = 0, secondLabelIndex = 0;
    
    char *outputFileName = (char *)calloc(strlen(srcFile), sizeof(char));
    strcpy(outputFileName, srcFile);
    /* Output file name would be <name>.ob\0 instead of <name>.as\0 */
    outputFileName[strlen(outputFileName) - 2] = 'o';
    outputFileName[strlen(outputFileName) - 1] = 'b';

    obj = fopen(outputFileName, "w");

    fprintf(obj, "  %d %d", firstPassIC, firstPassDC);

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        /* If the line contains .entry or the line is empty - we'll skip it */
        if (strstr(line, ".extern") != NULL || strlen(line) == 1)
            continue;
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
            writtenBinaryLines = linesDataArray[linesDataIndex].binaryLinesWritten;
            firstLabelIndex = linesDataArray[linesDataIndex].firstLabelIndex;
            secondLabelIndex = linesDataArray[linesDataIndex].secondLabelIndex;
            if(firstLabelIndex == -1) numbersAmount = 1;
            else if(secondLabelIndex == -1) numbersAmount = 2;
            else numbersAmount = 3;
            linesDataIndex++;
            /* If there are no more numbers, we'll just write the lines to the obj file */
            if (numbersAmount == 1) {
                /* read numbers[0] lines from the tmp file and write them to the obj file */
                for(i = 0; i < writtenBinaryLines; i++)
                {
                    fgets(tmpLine, MAX_LINE_LENGTH, tmpFile);
                    base4 = convertToEncryptedBase4(tmpLine);
                    fprintf(obj, "\n0%d ", objLineNumber + 99);
                    fprintf(obj, "%s", base4);
                    free(base4);
                    objLineNumber++;
                }
            }
            else if(numbersAmount == 2)
            {
                /* read numbers[1]-1 lines from the tmp file and write them to the obj file */
                for(i = 0; i < firstLabelIndex-1; i++)
                {
                    fgets(tmpLine, MAX_LINE_LENGTH, tmpFile);
                    base4 = convertToEncryptedBase4(tmpLine);
                    fprintf(obj, "\n0%d ", objLineNumber + 99);
                    fprintf(obj, "%s", base4);
                    free(base4);
                    objLineNumber++;
                }
                label = getLabelFromLine(line, firstLabelIndex-1);
                if(label[strlen(label)-1] == '\n')
                    label[strlen(label)-1] = '\0';
                found = 0;
                current = searchNodeInList(symbolTableHead, label, &found);
                if (found) {
                    if(strcmp(current->data, "external") != 0)
                    {
                        binaryLine = (char *)calloc(15, sizeof(char));
                        binaryNumber = intToBinary(current->line, 12);
                        strcat(binaryLine, binaryNumber);
                        strcat(binaryLine, "10");
                        base4 = convertToEncryptedBase4(binaryLine);
                        fprintf(obj, "\n0%d ", objLineNumber + 99);
                        fprintf(obj, "%s", base4);
                        free(binaryNumber);
                        free(base4);
                        free(binaryLine);

                        objLineNumber++;
                    }

                    /* If the label is external - we'll add it to the externs array
                    it's value will be the line number + 100 and it's label will be the label */
                    if(strcmp(current->data, "external") == 0) {
                        strcpy(tmpLine, "00000000000001");
                        base4 = convertToEncryptedBase4(tmpLine);
                        fprintf(obj, "\n0%d ", objLineNumber + 99);
                        fprintf(obj, "%s", base4);
                        free(base4);
                        objLineNumber++;
                        externsArray = (EntryExtern *)realloc(externsArray, (sizeof(EntryExtern) * (externsCount + 1)));
                        externsArray[externsCount].label = current -> name;
                        externsArray[externsCount].value = objLineNumber + 98;
                        externsCount++;
                    }
                }
                /* read the rest of the lines from the tmp file and write them to the obj file */
                for(i = 0; i < writtenBinaryLines - firstLabelIndex; i++){
                    fgets(tmpLine, MAX_LINE_LENGTH, tmpFile);
                    base4 = convertToEncryptedBase4(tmpLine);
                    fprintf(obj, "\n0%d ", objLineNumber + 99);
                    fprintf(obj, "%s", base4);
                    free(base4);
                    objLineNumber++;
                }
            }
            else if(numbersAmount == 3)
            {
                /* read numbers[1]-1 lines from the tmp file and write them to the obj file */
                for(i = 0; i < firstLabelIndex-1; i++) {
                    fgets(tmpLine, MAX_LINE_LENGTH, tmpFile);
                    base4 = convertToEncryptedBase4(tmpLine);
                    fprintf(obj, "\n0%d ", objLineNumber + 99);
                    fprintf(obj, "%s", base4);
                    free(base4);
                    objLineNumber++;
                }
                label = getLabelFromLine(line, firstLabelIndex-1);
                if(label[strlen(label)-1] == '\n')
                    label[strlen(label)-1] = '\0';
                found = 0;
                current = searchNodeInList(symbolTableHead, label, &found);
                if (found) {
                    /* Translate the label to binary - if it's not external */
                    if(strcmp(current->data, "external") != 0)
                    {
                        binaryLine = (char *)calloc(15, sizeof(char));
                        binaryNumber = intToBinary(current->line, 12);
                        strcat(binaryLine, binaryNumber);
                        strcat(binaryLine, "10");
                        base4 = convertToEncryptedBase4(binaryLine);
                        fprintf(obj, "\n0%d ", objLineNumber + 99);
                        fprintf(obj, "%s", base4);
                        free(binaryNumber);
                        free(base4);
                        free(binaryLine);

                        objLineNumber++;
                    }

                    /* If the label is external - we'll add it to the externs array
                    it's value will be the line number + 100 and it's label will be the label */
                    if(strcmp(current->data, "external") == 0) {
                        strcpy(tmpLine, "00000000000001");
                        base4 = convertToEncryptedBase4(tmpLine);
                        fprintf(obj, "\n0%d ", objLineNumber + 99);
                        fprintf(obj, "%s", base4);
                        free(base4);
                        objLineNumber++;
                        externsArray = (EntryExtern *)realloc(externsArray, (sizeof(EntryExtern) * (externsCount + 1)));
                        externsArray[externsCount].label = current -> name;
                        externsArray[externsCount].value = objLineNumber + 98;
                        externsCount++;
                    }
                }
                /* read numbers[2]-numbers[1]-1 lines from the tmp file and write them to the obj file */
                for(i = 0; i < writtenBinaryLines - secondLabelIndex-1; i++) {
                    fgets(tmpLine, MAX_LINE_LENGTH, tmpFile);
                    base4 = convertToEncryptedBase4(tmpLine);
                    fprintf(obj, "\n0%d ", objLineNumber + 99);
                    fprintf(obj, "%s", base4);
                    free(base4);
                    objLineNumber++;
                }
                label = getLabelFromLine(line, secondLabelIndex-1);
                if(label[strlen(label)-1] == '\n')
                    label[strlen(label)-1] = '\0';
                found = 0;
                current = searchNodeInList(symbolTableHead, label, &found);
                if (found) {
                    if(strcmp(current->data, "external") != 0)
                    {
                        binaryLine = (char *)calloc(15, sizeof(char));
                        binaryNumber = intToBinary(current->line, 12);
                        strcat(binaryLine, binaryNumber);
                        strcat(binaryLine, "10");
                        base4 = convertToEncryptedBase4(binaryLine);
                        fprintf(obj, "\n0%d ", objLineNumber + 99);
                        fprintf(obj, "%s", base4);
                        free(binaryNumber);
                        free(base4);
                        free(binaryLine);

                        objLineNumber++;
                    }

                    /* If the label is external - we'll add it to the externs array
                    it's value will be the line number + 100 and it's label will be the label */
                    if(strcmp(current->data, "external") == 0) {
                        strcpy(tmpLine, "00000000000001");
                        base4 = convertToEncryptedBase4(tmpLine);
                        fprintf(obj, "\n0%d ", objLineNumber + 99);
                        fprintf(obj, "%s", base4);
                        free(base4);
                        objLineNumber++;
                        externsArray = (EntryExtern *)realloc(externsArray, (sizeof(EntryExtern) * (externsCount + 1)));
                        externsArray[externsCount].label = current -> name;
                        externsArray[externsCount].value = objLineNumber + 98;
                        externsCount++;
                    }
                }
                /* read the rest of the lines from the tmp file and write them to the obj file */
                for(i = 0; i < writtenBinaryLines - secondLabelIndex-1; i++) {
                    fgets(tmpLine, MAX_LINE_LENGTH, tmpFile);
                    base4 = convertToEncryptedBase4(tmpLine);
                    fprintf(obj, "\n0%d ", objLineNumber + 99);
                    fprintf(obj, "%s", base4);
                    free(base4);
                    objLineNumber++;
                }
            }
        }
        else if(isEntry(line) == 1)
        {
            /* change the labels type to entry */
            label = getLabelFromLine(line, 1);
            if(label[strlen(label)-1] == '\n')
                label[strlen(label)-1] = '\0';
            found = 0;
            current = searchNodeInList(symbolTableHead, label, &found);
            if (found) {
                current->data = "entry";
            }
            free(label);
        }
    }

    fclose(file);
    fclose(obj);
    fclose(tmpFile);
    fclose(lineNumbers);

    remove(tmpFileName);
    /*remove("lineNumbers.txt");*/

    free(outputFileName);

    /* Run on the symbol table - only if there are labels of type entry *create* a .ent file */
    current = symbolTableHead;
    entryFileCreated = 0;
    entriesCount = 0;
    /* Create an entries array using the EntryExtern struct */
    entriesArray = (EntryExtern *)calloc(1, sizeof(EntryExtern));
    while(current != NULL) {
        if(strcmp(current->data, "entry") == 0) {
            if(!entryFileCreated)
            {
                /* Create the file */
                entryFileName = (char *)calloc(strlen(srcFile) + 5, sizeof(char));
                strcpy(entryFileName, srcFile);
                /* Remove the .am extension */
                entryFileName[strlen(entryFileName) - 3] = '\0';
                strcat(entryFileName, ".ent");
                entryFile = fopen(entryFileName, "w");
                free(entryFileName);
                entryFileCreated = 1;
            }
            entriesArray = (EntryExtern *)realloc(entriesArray, (sizeof(EntryExtern) * (i + 1)));
            entriesArray[entriesCount].label = current -> name;
            entriesArray[entriesCount].value = current -> line;
            entriesCount++;
        }
        current = current->next;
    }

    mergeSort(entriesArray, 0, entriesCount - 1);

    for(i = 0; i < entriesCount; i++) {
        if(i == 0) fprintf(entryFile, "%s %d", entriesArray[i].label, entriesArray[i].value);
        else fprintf(entryFile, "\n%s %d", entriesArray[i].label, entriesArray[i].value);
    }

    if(externsCount > 0) {
        /* Create the file */
        externFileName = (char *)calloc(strlen(srcFile) + 5, sizeof(char));
        strcpy(externFileName, srcFile);
        /* Remove the .am extension */
        externFileName[strlen(externFileName) - 3] = '\0';
        strcat(externFileName, ".ext");
        externFile = fopen(externFileName, "w");
        free(externFileName);

        for(i = 0; i < externsCount; i++) {
            if(i == 0) fprintf(externFile, "%s %d", externsArray[i].label, externsArray[i].value);
            else fprintf(externFile, "\n%s %d", externsArray[i].label, externsArray[i].value);
        }
    }

    free(entriesArray);
    free(externsArray);

    if(entryFileCreated)
        fclose(entryFile);
    if(externsCount > 0)
        fclose(externFile);

    free(entryFile);
    free(externFile);
}