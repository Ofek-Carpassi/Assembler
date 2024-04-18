#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "../Headers/dataStructers.h"
#include "../Headers/errors.h"

/**
 * @brief A function to create a new node
 * 
 * The function creates a new node with the given name, data and line number.
 * The function allocates memory for the new node and its fields.
 * The function returns a pointer to the new node.
 * 
 * @param name The name of the new node
 * @param data The data to be stored in the new node
 * @param line The line number of the new node
 * @return Node* A pointer to the new nodes
 */
Node *createNode(char *name, char *data, int value)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL)
    {
        return NULL;
    }
    newNode->name = (char *)malloc(strlen(name) + 1);
    if (newNode->name == NULL)
    {
        free(newNode);
        return NULL;
    }
    newNode->data = (char *)malloc(strlen(data) + 1);
    if (newNode->data == NULL)
    {
        free(newNode->name);
        free(newNode);
        return NULL;
    }
    strcpy(newNode->name, name);
    strcpy(newNode->data, data);
    newNode->line = value;
    newNode->next = NULL;
    return newNode;
}

/**
 * @brief A function to search for a node in a linked list
 * 
 * The function searches for a node with the given name in the given linked list.
 * The function returns a pointer to the node if found, otherwise it returns NULL.
 * The function also sets the value of the found parameter to 1 if the node is found, otherwise it sets it to 0.
 * The function time complexity is O(n) where n is the number of nodes in the list.
 * 
 * @param head A pointer to the head of the linked list
 * @param name The name of the node to search for
 * @param found A pointer to an integer to store the result of the search
 * @return Node* A pointer to the node if found, otherwise it returns NULL
 */
Node *searchNodeInList(Node *head, char *name, int *found)
{
    Node *current = head;
    while (current != NULL)
    {
        printf("current->name: %s\n", current->name);
        printf("name: %s\n", name);
        printf("strcmp: %d\n", strcmp(current->name, name));
        printf("strlen: %d\n", strlen(current->name));
        printf("strlen: %d\n", strlen(name));

        for(int i = 0; i < strlen(current->name); i++)
        {
            printf("current->name[%d]: %d\n", i, current->name[i]);
        }
        for(int i = 0; i < strlen(name); i++)
        {
            printf("name[%d]: %d\n", i, name[i]);
        }
        if (strcmp(current->name, name) == 0)
        {
            *found = 1;
            return current;
        }
        current = current->next;
    }
    *found = 0;
    return NULL;
}

/**
 * @brief A function to add a new node to a linked list
 * 
 * The function adds a new node with the given name, data and line number to the given linked list.
 * The function allocates memory for the new node and its fields.
 * 
 * @param head A pointer to the head of the linked list
 * @param name The name of the new node
 * @param data The data to be stored in the new node
 * @param line The line number of the new node
 */
void addNode(Node **head, char *name, char *data, int line)
{
    Node *newNode = createNode(name, data, line);
    if (newNode == NULL)
    {
        printIntError(ERROR_CODE_10);
        return;
    }
    newNode->next = *head;
    *head = newNode;
}

/**
 * @brief A function to free the memory allocated for a node
 * 
 * The function frees the memory allocated for the given nodes fields and the node itself.
 * 
 * @param node A pointer to the node to be freed
 */
void freeNode(Node *node)
{
    free(node->name);
    free(node->data);
    free(node);
}

/**
 * @brief A function to free the memory allocated for a linked list
 * 
 * The function frees the memory allocated for each node in the given linked list.
 * The function also sets the head of the list to NULL.
 * 
 * @param head A pointer to the head of the linked list
 */
void freeList(Node **head)
{
    Node *current = *head;
    Node *next;
    while (current != NULL)
    {
        next = current->next;
        freeNode(current);
        current = next;
    }
    *head = NULL;
}

void printList(Node *head)
{
    Node *current = head;
    while (current != NULL)
    {
        printf("Name: %s, Data: %s, Line: %d\n", current->name, current->data, current->line);
        current = current->next;
    }
}