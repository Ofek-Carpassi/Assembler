#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/dataStructures.h"
#include "../Headers/errors.h"

Node* createNode(char* name, char* data, int line) {
    /* Allocate memory for the new node */
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        return NULL;
    }

    /* Allocate memory for the name and data */
    newNode->name = (char*)malloc(strlen(name) + 1); 
    if (newNode->name == NULL) {
        free(newNode);
        printIntError(ERROR_CODE_10);
        exit(1);
        return NULL;
    }
	if(name[strlen(name)-1] == '\n') name[strlen(name)-1] = '\0';
    /* Save the name of the node */
    strcpy(newNode->name, name);

    newNode->data = (char*)malloc(strlen(data) + 1); 
    if (newNode->data == NULL) {
        free(newNode->name);
        free(newNode);
        printIntError(ERROR_CODE_10);
        exit(1);
        return NULL;
    }
    /* Save the data of the node */
    strcpy(newNode->data, data);

    /* Save the line of the node */
    newNode->line = line;
    newNode->next = NULL;
    /* Return the new node */
    return newNode;
}

/* This function searches for a node in the list */
Node* searchNodeInList(Node* head, char* name, int *found) {
    /* Initialize the current node */
    Node* current = head;
	int i = 0;
  /* Search for the node */
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
    /* If the node was not found, return NULL */
    *found = 0;
    return NULL;
}

/* This function adds a new node to the list */
void addNode(Node** head, char* name, char* data, int line) {
    /* Create a new node */
    Node* newNode = createNode(name, data, line);
    if (newNode == NULL) {
        printIntError(ERROR_CODE_10);
        return;
    }

    /* Add the new node to the list */
    newNode->next = *head;
    *head = newNode;
}

/* This function frees a node */
void freeNode(Node* node) {
    free(node->name);
    free(node->data);
    free(node);
}

/* This function frees the list */
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
    /* Allocate memory for the new lineData */
    lineData *newLineData = (lineData*)malloc(sizeof(lineData));
    if (newLineData == NULL) {
        return NULL;
    }

    /* Save the binaryLinesWritten, firstLabelIndex, and secondLabelIndex */
    newLineData->binaryLinesWritten = binaryLinesWritten;
    newLineData->firstLabelIndex = firstLabelIndex;
    newLineData->secondLabelIndex = secondLabelIndex;
    /* Return the new lineData */
    return newLineData;
}

/* This function adds a new line to the array */
void addLine(lineData **array, int binaryLinesWritten, int firstLabelIndex, int secondLabelIndex, int *arraySize) {
    /* Create a new lineData */
    lineData *newLineData = createLineData(binaryLinesWritten, firstLabelIndex, secondLabelIndex);
    if (newLineData == NULL) {
        printIntError(ERROR_CODE_10);
        return;
    }

    /* Add the newLineData to the array */
    if(*arraySize != 0) {
        /* Reallocate memory for the array */
        lineData* temp = (lineData*)realloc(*array, (*arraySize + 1) * sizeof(lineData));
        if (temp == NULL) {
            printIntError(ERROR_CODE_10);
            free(newLineData);
            return;
        }
        /* Save the newLineData */
        *array = temp;
    }
    if (*array == NULL) {
        printIntError(ERROR_CODE_10);
        free(newLineData);
        return;
    }

    /* Save the newLineData */
    if(arraySize!=0)
    {
        (*array)[*arraySize] = *newLineData;
        (*arraySize)++;
    }
    else
        (*array)[*arraySize] = *newLineData;

    /* Free the newLineData */
    free(newLineData);
}

