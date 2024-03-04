#ifndef PREASSEM_H
#define PREASSEM_H

#include <stdio.h>
#include "dataStructers.h"

/**
 * @brief This function is used to clean a line from white spaces.
 * 
 * This function will clean a line from white spaces and return the cleaned line.
 * The function adds a space at the end of the line and returns the cleaned line.
 * 
 * @param line - a string containing the line to clean.
 * @return - a string containing the cleaned line.
 */
char *cleanLine(char *line);

/**
 * @brief This function is used to check if a line is a declaration of a macro.
 * 
 * This function will check if a line is a declaration of a macro by parsing the first word in the line and checking if it's "mcr".
 * The function will return 1 if the line is a declaration of a macro, 0 otherwise.
 * 
 * @param line - a string containing the line to check.
 * @return - 1 if the line is a declaration of a macro, 0 otherwise.
 */
int isMacroDeclaration(char *line);

/**
 * @brief This function is used to get the name of a macro from a line of a macro declaration.
 * 
 * This function will parse the line and return the name of the macro (the second word in the line).
 * 
 * @param line - a string containing the line to parse.
 * @return - a string containing the name of the macro.
 */
char *getMacroName(char *line);

/**
 * @brief This function is used to save a macro to a linked list.
 * 
 * This function will save a macro to a linked list.
 * This function opens the file and reads the lines in the declaration of the macro (not the mcr and not the endmcr).
 * The function will save the macro to the linked list and return 1 if the macro was saved successfully, 0 otherwise.
 * 
 * @param file - a string containing the file name to read from.
 * @param head - a pointer to the head of the linked list.
 * @param lineNumber - a pointer to an integer containing the line the macro is declared in (the line with the mcr).
 * @param name - a string containing the name of the macro.
 * @return - 1 if the macro was saved successfully, 0 otherwise.
 */
int saveMacroToList(char *file, Node **head, int *lineNumber, char *name);

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
* Execute the pre assembler (the macro expansion) for the source file.
*
* 1. Create the new file
* 2. Get each line with fgets in a while not EOF
* 3. Clean the line from white spaces and check if the line's syntax is valid
* 4. If the line is a call to a macro - call to replaceMacroCall and save the new line to a new file
* 5. If the line is a declaration of a macro - call to addMacro (which calls to isValidDeclarationMacro) and save the macro to a linked list
* 6. If the line isn't a macro call or declaration - copy the line to the new file
* 7. Free the memory and close the files
*
* @param file - a string containing the file name to read from.
* @return - 1 if the pre assembler executed successfully, 0 otherwise.
*/
void executePreAssembler(char *file, char **outputFileName);

#endif

/*
We need to
Start by working with a one lined file and then move to a multi lined file.
We need to make sure that the flow of the program is correct and that the functions are working as expected.
Then we can move to the next step - working with a multi lined file.
*/

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