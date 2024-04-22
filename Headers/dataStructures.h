#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

typedef struct Node {
    char* name;
    char* data;
    unsigned int line;
    struct Node* next;
} Node;

Node* createNode(char* name, char* data, int line);
Node* searchNodeInList(Node* head, const char* name, int *found);
void addNode(Node** head, char* name, char* data, int line);
void freeNode(Node* node);
void freeList(Node* head);
void printList(Node* head);

#endif