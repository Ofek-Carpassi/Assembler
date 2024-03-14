#ifndef DATASTRUCTERS_H
#define DATASTRUCTERS_H

/* A node in the linked list. */
typedef struct Node {
    char *name;         /* The name associated with the node. */
    char *data;         /* The data stored in the node. */
    int line;           /* The line number of the data. */
    struct Node *next;  /* The next node in the list. */
} Node;

/**
* This function creates a new node for a macro.
*   
* @param name The name of the macro from the input file.
* @param data The data of the macro from the input file.
* @param line The line number in the input file where the macro was created.
* @return A pointer to the new node.
*/
Node *createNode(char *name, char *data, int line);

/**
* This function searches a macro in the linked list.
*
* @param head The head of the linked list.
* @param name The name of the macro to search for.
* @param found A pointer to an integer that will be set to 1 if the macro is found, 0 otherwise.
* @return A pointer to the node containing the macro, or NULL if the macro is not found.
*/
Node *searchNodeInList(Node *head, char *name, int *found);

/**
* This function adds a new node to the linked list.
*
* @param head A pointer to the head of the linked list.
* @param name The name of the macro to add.
* @param data The data of the macro to add.
* @param line The line number in the input file where the macro was created.
*/
void addNode(Node **head, char *name, char *data, int line);

/**
* This function frees the memory of a node and its data.
*
* @param node A pointer to the node to free.
*/
void freeNode(Node *node);

/**
* This function frees the memory of the entire linked list.
*
* @param head A pointer to the head of the linked list.
*/
void freeList(Node **head);

#endif