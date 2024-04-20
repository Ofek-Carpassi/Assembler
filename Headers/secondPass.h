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

/*
Algorithm - written codly so we know which functions we need to write:
1. Initialize IC (Instruction Counter) to 0.
2. Read a line from the input file - if the input has ended, go to step 8. (executeSecondPass - loops through the file)
3. Check if the first field is a label. (checkFieldLabel)
4. What it starts with? (checkStart) (if .data, .string or .extern go to step 2 - if .entry go to step 6)
5. In the operation table, put the right label in the .entry field. (handleEntry)
6. Complete the operands translation according to the addressing method. (translateOperands)
7. Update IC by L. Go to step 2. (continue the loop)
8. The input file was read, if there were any errors on the second pass, stop here.
9. Build the output files. (buildOutputFiles - another file).
*/

/**
 * @brief This function translates operands to the output file of the binary.
 * 
 * This function completes the operands translation according to the addressing method.
 * The function writes the binary code to the file.
 * The function returns 1 if the translation was handled successfuly and 0 otherwise.
 * 
 * @param symbolTableHead - The head of the operation table
 * @param file - The machine code file to write the machine code to.
 * @return 1 if the translation was handled successfuly, 0 otherwise.
 */
int translateOperands(Node **symbolTableHead, FILE *file);

/**
 * @brief This function handles entry and executes step 5 in the algorithm.
 * 
 * @param symbolTableHead - The head of the operation table
 * @return This function return 1 if the handling was executing succefuly, 0 otherwise.
 */
int handleEntry(Node **symbolTableHead);

/**
 * @brief This function check what is the start of a line.
 * 
 * This function checks checks what a line starts with.
 * The function will return the data if .data, string if .string and more.
 * 
 * @param line - The line to check
 * @return The function will return the type in a string.
 */
char *checkStart(char *line);

/**
 * @brief This function checks the type of the first field.
 * 
 * This function checks what is the first field in a line.
 * If the field is a label, the function will return 1.
 * If the field is illegal the function will return 0.
 * 
 * @param line - the line to check
 * @return The function returns 1 if the first field is a label, 0 otherwise.
 */
int checkFieldLabel(char *line);


int isEntry(char *line);

char *convertToEncryptedBase4(char *binaryLine);

int hasLabel(char *line);

/**
 * @brief This function executes the second pass algorithm.
 * 
 * @param file - The input file to read from.
 * @param outputFileName - The name of the output file.
 */
void executeSecondPass(char *srcFile, char *tmpFileName, Node *symbolTableHead);


#endif