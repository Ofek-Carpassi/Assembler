#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

typedef struct Node {
    char* name;
    char* data;
    unsigned int line;
    struct Node* next;
} Node;

typedef struct lineNumbers {
    unsigned int binaryLinesWritten;
    unsigned int firstLabelIndex;
    unsigned int secondLabelIndex;
} lineNumbers;

typedef struct OPCODE {
    char *name;
    char *binary;
    int arguments;
} Opcode;

typedef struct entryExtern {
    char *label;
    int value;
} EntryExtern;

typedef struct Error {
    int errorID;        /* The ID of the error. */
    char *errorString;  /* The string of the error. */
} Error;

typedef struct location {
    char *fileName;
    int line;
} location;

Node* createNode(char* name, char* data, int line);
Node* searchNodeInList(Node* head, const char* name, int *found);
void addNode(Node** head, char* name, char* data, int line);
void freeNode(Node* node);
void freeList(Node* head);
void printList(Node* head);

#endif