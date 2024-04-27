/*
The pre assembler algorithm will work as follows:
1. preAssemblerExecuter will be called with the source file.
2. Create a new file to write to.
3. Read the source file line by line.
4. If the line is EOF - go to step 6.
5. After each line is read the function will do the following:
    a. Clean the line from white spaces using "cleanLine" and return the cleaned line.
    b. Check if the line is a macro call or macro declaration.
    c. If the line isn't a macro call or macro declaration - copy the line to the new file using "copyLineToFile" and return to step 3.
    d. If the line is a macro call - replace the macro call with the macro's definition using "replaceMacroCall" and return to step 3.
    e. If the line is a macro declaration - check if the macro's name is valid using "isValidMacroName" and add the macro to the macros linked list using "saveMacroToList" and return to step 3.
    f. If the line is a comment (starts with ';') - ignore the line and return to step 3.
6. Free the memory and close the files.
*/

#ifndef PREASSEM_H
#define PREASSEM_H

#include <stdio.h>
#include "dataStructures.h"

/**
 * @brief This function is used to check if a line is a macro declaration.
 * 
 * This function will check if the line contains the word "mcr" and return 1 if it does, 0 otherwise.
 * 
 * @param line - a string containing the line to check.
 * @return - 1 if the line is a macro declaration, 0 otherwise.
 */
int isMacroDeclaration(char *line);

/**
 * @brief This function returns the name of the macro.
 * 
 * This function will get the second word in the line and return it.
 * 
 * @param line - a string containing the line to get the macro name from.
 * @return - a string containing the name of the macro.
 */
char *getMacroName(char *line);

/**
 * @brief This function is used to save a macro to the macros linked list.
 * 
 * This function will get the name of the macro and the line number of the macro's declaration.
 * The function will create a new node and save the macro's name, line number and the macro's definition to the node.
 * The function reads the macro's definition from the file and saves it to the node.
 * 
 * @param file - a string containing the name of the file to read from.
 * @param head - a pointer to a pointer to the head of the macros linked list.
 * @param lineNumber - an integer containing the line number of the macro's declaration.
 * @param name - a string containing the name of the macro.
 */
void saveMacroToList(char *file, Node **head,char *name);

/**
 * @brief This function is used to check if a macro's name is valid.
 * 
 * This function uses an array of invalid names and checks if the macro's name is not in the array.
 * The array holds the names of all opcodes and registers (r0-r7).
 * The function will return 1 if the macro's name is valid, 0 otherwise.
 * 
 * @param name - a string containing the name of the macro to check.
 * @return - 1 if the macro's name is valid, 0 otherwise.
 */
int isValidMacroName(char *name);

/**
 * @brief This function is used to replace a macro call with the macro's definition.
 * 
 * This function will get a node containing a macro and will copy the macro's definition to the new file.
 * This function uses the data in the node and copies each line in the data to the .am file.
 * The function will return 1 if the macro call was replaced successfully, 0 otherwise.
 * 
 * @param macro - a pointer to a node containing the macro to replace.
 * @param file - the name of the file to write to.
 * @return - 1 if the macro call was replaced successfully, 0 otherwise.
 */
int *replaceMacroCall(Node *macro, char *file);

/**
 * @brief This function is used to check if a line is a call to a macro.
 * 
 * This function will check if the line contains one word and if the word is a macro's name.
 * 
 * @param line - a string containing the line to check.
 * @param head - a pointer to a pointer to the head of the macros linked list.
 * @return - 1 if the line is a call to a macro, 0 otherwise.
 */
int isCallToMacro(char *line, Node **head);

/**
 * @brief This function is used to clean a line from white spaces.
 * 
 * This function reads the input file, line by line and handle it using the pre assembler algorithm.
 * 
 * @param file - a string containing the name of the file to read from.
 * @param outputFileName - a pointer to a string - the function will allocate memory for the string and save the name of the new file to it.
 */
void executePreAssembler(char *file, char **outputFileName);

#endif