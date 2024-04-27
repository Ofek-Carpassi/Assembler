#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

/* A struct used to store the data of a node in a linked list. */
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

/* A struct to save all the entries and externs in the file. */
typedef struct entryExtern {
    char *label;
    int value;
} EntryExtern;

/* A struct for the error. */
typedef struct Error {
    int errorID;        /* The ID of the error. */
    char *errorString;  /* The string of the error. */
} Error;

/* A struct to save the location of the error. */
typedef struct location {
    char *fileName;
    int line;
} location;

/* A struct to save data about each line in the file. */
typedef struct lineData {
    int binaryLinesWritten;
    int firstLabelIndex;
    int secondLabelIndex;
} lineData;

/* A function to create a new node. */
Node* createNode(char* name, char* data, int line);
/* A function to search for a node in a linked list. */
Node* searchNodeInList(Node* head,  char* name, int *found);
/* A function to add a node to a linked list. */
void addNode(Node** head, char* name, char* data, int line);
/* A function to free a node. */
void freeNode(Node* node);
/* A function to free a linked list. */
void freeList(Node* head);
/* A function to create a new line data. */
lineData *createLineData(int binaryLinesWritten, int firstLabelIndex, int secondLabelIndex);
/* A function to add a line into an array of line data. */
void addLine(lineData **head, int binaryLinesWritten, int firstLabelIndex, int secondLabelIndex, int *line);

#endif
