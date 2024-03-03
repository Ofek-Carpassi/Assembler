#ifndef PREASSEM_H
#define PREASSEM_H

#include <stdio.h>
#include "DataStructers.h"

/**
 * @brief This function is used to clean and organize a line.
 * 
 * This function will use an algorithm written in the matrices calculator project to clean the line from white spaces and organize it.
 * 
 * @param line - a string containing the line to clean.
 * @return - a string containing the cleaned line.
 */
char *cleanLine(char *line);

/**
 * @brief This function is used to copy a line to the new file.
 * 
 * This function will copy a line to the new file and return 1 if the line was copied successfully, 0 otherwise.
 * 
 * @param file - the file to write to.
 * @param line - a string containing the line to copy.
 * @return - 1 if the line was copied successfully, 0 otherwise.
 */
int copyLineToFile(FILE *file, char *line);

/**
 * @brief This function is used to add a macro to the macros linked list.
 * 
 * This function will add a macro to the macros linked list and return 1 if the macro was added successfully, 0 otherwise.
 * 
 * @param file - a string containing the file name to read from.
 * @param head - a pointer to the head of the linked list.
 * @return - 1 if the macro was added successfully, 0 otherwise.
 */
int saveMacroToList(char *file, node **head);

/**
 * @brief This function is used to check if the macro's name is valid.
 * 
 * This function parses the macro's name and checks if it's valid.
 * 
 * @param name - a string containing the macro line to check.
 * @return - 1 if the macro's name is valid, 0 otherwise.
 */
int isValidMacroName(char *line);

/**
 * @brief This function is used to replace a macro call with the macro's definition (copy the macro's definition to the new file).
 * 
 * This function will get a node containing a macro and will copy the macro's definition to the new file.
 * 
 * @param macro - a pointer to a node containing the macro to replace.
 * @param file - the file to write to.
 * @return - 1 if the macro call was replaced successfully, 0 otherwise.
 */
int *replaceMacroCall(node *macro, FILE *file);

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
int preAssemblerExecuter(char *file);

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
6. Free the memory and close the files.
*/