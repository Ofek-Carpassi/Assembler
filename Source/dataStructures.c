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

    newNode->name = name;
    if (newNode->name == NULL) {
        free(newNode);
        return NULL;
    }

    newNode->data = data;
    if (newNode->data == NULL) {
        free(newNode->name);
        free(newNode);
        return NULL;
    }

    newNode->line = line;
    newNode->next = NULL;
    return newNode;
}

Node* searchNodeInList(Node* head, const char* name, int *found) {
    Node* current = head;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
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