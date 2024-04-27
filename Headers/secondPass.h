#ifndef SECONDPASS_H
#define SECONDPASS_H

#include <stdio.h>
#include "dataStructures.h"

/*
Algorithm:
1. Initialize IC (Instruction Counter) to 0.
2. Read a line from the input file - if the input has ended, go to step 9.
3. If the first field is a label, ignore it.
4. Is it a .data, .string or .extern? If so, go to step 2.
5. Is it a .entry? If so go to step 9.
6. In the operation table, put the right label in the .entry field. go to step 2.
7. Complete the operands translation according to the addressing method.
    If the operand contains a label, find it in the symbol table and code it to the memory.
8. Update IC by L. Go to step 2.
9. The input file was read, if there were any errors on the second pass, stop here.
10. Build the output files.
*/

/* This function returns "strstr(line, ".entry") != NULL" to check if the line is an entry. */
int isEntry(char *line);

/**
 * @brief This function gets a binary line and converts it to encrypted base 4.
 * 
 * @param binaryLine - The binary line to convert.
 * @return The function returns the encrypted base 4 line.
 */
char *convertToEncryptedBase4(char *binaryLine);

/* This function returns "strstr(line, ":") != NULL" to check if the line has a label declaration. */
int hasLabel(char *line);

/**
 * @brief This function gets a line and returns the label from it.
 * 
 * @param line - The line to get the label from.
 * @param index - The index of the label in the line.
 * @return The function returns the label from the line.
 */
char *getLabelFromLine(char *line, int index);

/**
 * @brief This function executes the merge part of the merge sort algorithm on an array of EntryExtern.
 * 
 * @param arr - The array to sort.
 * @param l - The left index.
 * @param m - The middle index.
 * @param r - The right index.
 */
void merge(EntryExtern *arr, int l, int m, int r);

/**
 * @brief This function executes the merge sort algorithm on an array of EntryExtern.
 * 
 * @param arr - The array to sort.
 * @param l - The left index.
 * @param r - The right index.
 */
void mergeSort(EntryExtern *arr, int l, int r);

/**
 * @brief This function executes the second pass of the assembler.
 * 
 * @param srcFile - The source file name.
 * @param tmpFileName - The temporary file name.
 * @param symbolTableHead - The head of the symbol table.
 * @param firstPassIC - The IC from the first pass.
 * @param firstPassDC - The DC from the first pass.
 * @param linesDataArray - The array of line data.
 */
void executeSecondPass(char *srcFile, char *tmpFileName, Node *symbolTableHead, int firstPassIC, int firstPassDC, lineData *linesDataArray);

#endif