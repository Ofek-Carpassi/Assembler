#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/dataStructures.h"
#include "../Headers/errors.h"

Node* createNode(char* name, char* data, int line) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        return NULL;
    }

    newNode->name = (char*)malloc(strlen(name) + 1); 
    if (newNode->name == NULL) {
        free(newNode);
        printIntError(ERROR_CODE_10);
        exit(1);
        return NULL;
    }
	if(name[strlen(name)-1] == '\n') name[strlen(name)-1] = '\0';
    strcpy(newNode->name, name);

    newNode->data = (char*)malloc(strlen(data) + 1); 
    if (newNode->data == NULL) {
        free(newNode->name);
        free(newNode);
        printIntError(ERROR_CODE_10);
        exit(1);
        return NULL;
    }
    strcpy(newNode->data, data);

    newNode->line = line;
    newNode->next = NULL;
    return newNode;
}

Node* searchNodeInList(Node* head, char* name, int *found) {
    Node* current = head;
	int i = 0;
    if(name[strlen(name) - 1] == ':' || name[strlen(name) - 1] == '\n') {
        name[strlen(name) - 1] = '\0';
    }
    while (current != NULL) {
        for(i = 0; i<strlen(current->name); i++)
			if(name[i] != (current->name)[i])
				break;
		if(i == strlen(current->name))
		{
			*found = 1;
			return current;
		}
        current = current->next;
    }
    *found = 0;
    return NULL;
}

void addNode(Node** head, char* name, char* data, int line) {
    Node* newNode = createNode(name, data, line);
    if (newNode == NULL) {
        printIntError(ERROR_CODE_10);
        return;
    }

    newNode->next = *head;
    *head = newNode;
}

void freeNode(Node* node) {
    free(node->name);
    free(node->data);
    free(node);
}

void freeList(Node* head) {
    Node* current = head;
    Node* next;
    while (current != NULL) {
        next = current->next;
        freeNode(current);
        current = next;
    }
}

void printList(Node* head) {
    Node* current = head;
    while (current != NULL) {
        printf("Name: %s, Data: %s, Line: %d\n", current->name, current->data, current->line);
        current = current->next;
    }
}

lineData *createLineData(int binaryLinesWritten, int firstLabelIndex, int secondLabelIndex) {
    lineData *newLineData = (lineData*)malloc(sizeof(lineData));
    if (newLineData == NULL) {
        return NULL;
    }

    newLineData->binaryLinesWritten = binaryLinesWritten;
    newLineData->firstLabelIndex = firstLabelIndex;
    newLineData->secondLabelIndex = secondLabelIndex;
    return newLineData;
}

void addLine(lineData **array, int binaryLinesWritten, int firstLabelIndex, int secondLabelIndex, int *arraySize) {
    lineData *newLineData = createLineData(binaryLinesWritten, firstLabelIndex, secondLabelIndex);
    if (newLineData == NULL) {
        printIntError(ERROR_CODE_10);
        return;
    }

    /* Add the newLineData to the array */
    if(*arraySize != 0) {
        lineData* temp = (lineData*)realloc(*array, (*arraySize + 1) * sizeof(lineData));
        if (temp == NULL) {
            printIntError(ERROR_CODE_10);
            free(newLineData);
            return;
        }
        *array = temp;
    }
    if (*array == NULL) {
        printIntError(ERROR_CODE_10);
        free(newLineData);
        return;
    }

    if(arraySize!=0)
    {
        (*array)[*arraySize] = *newLineData;
        (*arraySize)++;
    }
    else
        (*array)[*arraySize] = *newLineData;

    free(newLineData);
}
