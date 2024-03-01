#ifndef PREASSEM_H
#define PREASSEM_H

#include <stdio.h>
#include "DataStructers.h"

/**
* This function is used to copy the contents of a macro.
* 
* @param file - the file to read from.
* @param position - the position in the file to start reading from.
* @param lineCounter - the line counter to update.
* @return - a string containing the macro.
*/
char *saveMacroString(FILE *file, fpos_t *position, int *lineCounter);

/**
* This function is used to save a macro from a file.
*
* @param file - a string containing the file name to read from.
* @param head - a pointer to the head of the linked list.
* @return - 1 if all the macros were added successfully, 0 otherwise.
*/
int addMacro(char *file, node **head);

/**
* This function is used to check if a macro is valid.
*
* @param macro - a string containing the macro to check.
* @param name - a pointer to a string containing the name of the macro.
* @param lineCounter - the line which we are reading the macro from.
* @param file - a string containing the file name to read from.
* @return - 1 if the macro is valid, 0 otherwise.
*/
int isValidDeclarationMacro(char *macro, char **name, int lineCounter, char *file);

/**
* This function replaces all the macro calls in the source file and creates a new modified file.
*
* @param file - a string containing the file name to read from.
* @param head - a pointer to the head of the linked list.
* @return - a string containing the name of the new file.
*/
char *replaceAllMacros(char *file, node *head);

/**
* This function is used to remove all the macro declarations from the source file and creates a new modified file.
*
* @param file - a string containing the file name to read from.
* @return - a string containing the name of the new file.
*/
char *removeAllMacroDeclaration(char *file);

/**
* This function replaces a call to a macro with the definition and saves the line to a string.
*
* @param string - a string containing the line to replace.
* @param macro - a pointer to the macro to replace.
* @return - a string containing the new line.
*/
char *replaceMacroCall(char *string, node *macro);

/**
* This function is used to check if theres a macro call before the declaration.
*
* @param file - a string containing the file name to read from.
* @param head - a pointer to the head of the linked list.
* @return - 1 if there is a macro call before the declaration, 0 otherwise.
*/
int macroCallBeforeDeclaration(char *file, node *head);

/**
* Execute the pre assembler (the macro expansion) for the source file.
*
* This function executes the following steps:
* 1. Removing all white spaces from the source file and saving it to a new file.
* 2. Adding all the macros to a linked list.
* 3. Making sure there are no macro calls before the declaration.
* 4. Removing all the macro declarations from the source file and saving it to a new file.
* 5. Replacing all the macro calls with the definition and saving it to a new file.
* 6. Freeing memory and cleaning temporary files.
*
* @param file - a string containing the file name to read from.
* @return - 1 if the pre assembler executed successfully, 0 otherwise.
*/
int preAssemblerExecuter(char *file);

#endif