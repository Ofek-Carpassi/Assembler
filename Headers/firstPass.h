#ifndef FIRSTPASS_H
#define FIRSTPASS_H

#include <stdio.h>
#include "dataStructers.h"
/*
Algorithm:
1. IC (Instruction Counter) and DC (Data Counter) are initialized to 0.
2. Read a line from the input file - if the input has ended, go to step 17.
3. If the line is empty or a comment, go to step 2.
4. Is the line a constant declaration (starts with .define)? If not go to step 6.
5. Add the constant to the symbol table with mdefine as its type and its value as its value. Go to step 2.
6. Is the line a label declaration? If not go to step 8.
7. Turn flag "isLabel" on
8. Is it an instruction to save data (.data or .string)? If not go to step 11.
9. If there's a label declaration, add it to the symbol table with data type, its value is DC.
10. Find the type of data, code them to the memory and update DC by the length of the data.
    If the data is a .data and it has a value that is a constant, make sure the constant is in the symbol table with mdefine. Go to step 2.
11. Is it an instruction to save code (.entry or .extern)? If not go to step 13.
12. Is it a .extern? If so, add each label that shows as an operand to the symbol table without a value and with type external. Go to step 2.
13. Is it a label declaration? If so, add it to the symbol table with code type and its value is IC+100.
14. Find the operation name in the operation table and code it to the memory.
15. Analyze the operands structure and calculate L (amount of words the instruction takes). 
    Build the binary code of the first word of the instruction and code it to the memory. 
    If there's a second word, code it to the memory. Update IC by L.
16. Update IC by L. Go to step 2.
17. The input file was read, if there were any errors on the first pass, stop here.
18. Update the symbol table of each data type with the value of IC + 100.
19. Start the second pass.
*/

/*
Algorithm - written codly so we know which functions we need to write:
1. IC (Instruction Counter) and DC (Data Counter) are initialized to 0.
2. Read a line from the input file - if the input has ended, go to step 11. (executeFirstPass - loops through the file)
3. Check the type of the line. (checkLineType)
4. If the line is empty or a comment, go to step 2. (continue the loop)
5. Is the line a constant declaration? if so call handleConstantDeclaration. (handleConstantDeclaration)
6. Is the line a label declaration? if so call handleLabelDeclaration. (handleLabelDeclaration)
7. Is it an instruction to save data? if so call handleDataDeclaration. (handleDataDeclaration)
8. Is it an instruction to save code? if so call handleCodeDeclaration. (handleInstruction)
9. Calculate L (amount of words the instruction takes). (calcLength)
10. Update IC by L. Go to step 2. (continue the loop)
11. The input file was read, if there were any errors on the first pass, stop here.
12. Update the symbol table of each data type with the value of IC + 100.
13. Start the second pass.
*/

/**
 * @brief This function calculates the length of an instruction.
 * 
 * This function counts the amount of word in the instruction and returns it.
 * 
 * @param line The line to calculate the length of.
 * @return The amount of words in the instruction.
 */
int calcLength(char *line);

/**
 * @brief This function handles an instruction with two operands.
 * 
 * This function receives the two operands and a pointer to the head of the symbol table.
 * The function writes the binary code of the addressing method of the operands to the binary line.
 * The function returns 1 if the operation was successful, 0 otherwise.
 * 
 * @param operandOne The first operand.
 * @param operandTwo The second operand.
 * @param symbolTableHead A pointer to the head of the symbol table.
 * @param binaryLine A pointer to the binary line.
 * @return 1 if the operation was successful, 0 otherwise.
 */
int handleTwoOperands(char *operandOne, char *operandTwo, Node **symbolTableHead, char **binaryLine);

/**
 * @brief This function handles an instruction with one operand.
 * 
 * This function receives the operand and a pointer to the head of the symbol table.
 * The function writes the binary code of the addressing method of the operand to the binary line.
 * The function returns 1 if the operation was successful, 0 otherwise.
 * 
 * @param operand The operand.
 * @param symbolTableHead A pointer to the head of the symbol table.
 * @param binaryLine A pointer to the binary line.
 * @return 1 if the operation was successful, 0 otherwise.
 */
int handleOneOperand(char *operand, Node **symbolTableHead, char **binaryLine);

/**
 * @brief This function handles a code declaration.
 * 
 * This function adds the label to the symbol table with code as its type and its value as IC+100.
 * The function receives the line to handle and the symbol table (a pointer to the head of the linked list).
 * The function returns 1 if the label was added to the symbol table, 0 otherwise.
 * 
 * @param line The line to handle.
 * @param symbolTableHead A pointer to the head of the linked list.
 * @return 1 if the label was added to the symbol table, 0 otherwise.
 */
char *handleInstruction(char *line, Node **symbolTableHead);

/**
 * @brief This function handles a string.
 * 
 * This function gets a line with string declaration and translates the string to binary and writes it to the output file.
 * This function recieves the line to handle.
 * The function returns the string in binary.
 * 
 * @param line The line to handle.
 * @return The string in binary.
 */
char *handleString(char *line);

/**
 * @brief This function handles a data.
 * 
 * This function gets a line with data declaration and translates the data to binary and writes it to the output file.
 * The function receives the line to handle and the symbol table (a pointer to the head of the linked list).
 * The function returns the data in binary.
 * 
 * @param line The line to handle.
 * @param symbolTableHead A pointer to the head of the linked list.
 * @return The data in binary.
 */
char *handleData(char *line, Node **symbolTableHead);

/**
 * @brief This function handles a label.
 * 
 * This function adds the label to the symbol table with code as its type and its value as IC+100.
 * The function receives the line to handle and the symbol table (a pointer to the head of the linked list).
 * The function returns 1 if the label was added to the symbol table, 0 otherwise.
 * 
 * @param line The line to handle.
 * @param symbolTableHead A pointer to the head of the linked list.
 * @return 1 if the label was added to the symbol table, 0 otherwise.
 */
char *handleLabel(char *line, Node **symbolTableHead);

/**
 * @brief This function handles a constant.
 * 
 * This function adds the constant to the symbol table with mdefine as its type and its value as its value.
 * The function receives the line to handle and the symbol table (a pointer to the head of the linked list).
 * The function returns 1 if the constant was added to the symbol table, 0 otherwise.
 * 
 * @param line The line to handle.
 * @param symbolTableHead A pointer to the head of the linked list.
 * @return 1 if the constant was added to the symbol table, 0 otherwise.
 */
void handleConstant(char *line, Node **symbolTableHead);

/**
 * @brief This function checks the type of the line and returns it.
 * 
 * This function checks if the line is empty, a comment, a constant declaration, 
 * a label declaration, a data declaration, a string declaration, 
 * an entry declaration, an extern declaration, or an instruction.
 * The function returns a string with the type of the line.
 * The function also writes the line to the output file.
 * 
 * @param line The line to check.
 * @return 
 *          1 if the line is empty or a comment
 *          2 if the line is a constant declaration
 *          3 if the line is a label declaration
 *          4 if the line is a data declaration
 *          5 if the line is a string declaration
 *          6 if the line is an entry declaration
 *          7 if the line is an extern declaration
 *          8 if the line is an instruction
 *          0 if the line is illegal - an error
 */
int checkLineType(char *line);

/**
 * @brief This function executes the first pass algorithm
 * 
 * @param file - The input file to read from.
 * @param outputFileName - The name of the output file.
 */
void executeFirstPass(char *file, char **outputFileName);

#endif