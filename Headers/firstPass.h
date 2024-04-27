#ifndef FIRSTPASS_H
#define FIRSTPASS_H

#include <stdio.h>
#include "dataStructures.h"
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
    If there's a second word, code it to the memory.
16. Update IC by L. Go to step 2.
17. The input file was read, if there were any errors on the first pass, stop here.
18. Update the symbol table of each data type with the value of IC + 100.
19. Start the second pass.
*/

/* Check if an operand is illegal */
int isOperandIllegal(char *operand);

/* Check if a label is illegal */
int isLabelIllegal(char *label);

/* Check if a word is an instruction */
int isInstruction(char *word);

/**
 * @brief This function handles a code declaration.
 * 
 * This function converts any instruction to binary and returns it.
 * 
 * @param parsedLine The line parsed by the space.
 * @param symbolTableHead A pointer to the head of the symbol table.
 * @param wordAmount The amount of words in the line.
 * @return The binary code of the instruction.
 */
char *handleInstruction(char **parsedLine, Node **symbolTableHead, int wordAmount);

/**
 * @brief This function handles a string.
 * 
 * This function gets a line with string declaration and converts the string to binary and writes it to the output file.
 * 
 * @param parsedLine The line parsed by the space.
 * @param line The line to handle.
 * @param wordAmount The amount of words in the line.
 */
char* handleString(char **parsedLine, char *line, int wordAmount);

/**
 * @brief This function handles a data.
 * 
 * This function gets a line with data declaration and converts the data to binary and writes it to the output file.
 * 
 * @param parsedLine The line parsed by the space.
 * @param symbolTableHead A pointer to the head of the symbol table.
 * @param wordAmount The amount of words in the line.
 * @return The binary code of the data.
 */
char *handleData(char *parsedLine[], Node **symbolTableHead, int wordAmount);

/**
 * @brief This function handles a label.
 * 
 * This function adds the label to the symbol table with the type of the label.
 * 
 * @param label The label to handle.
 * @param symbolTableHead A pointer to the head of the symbol table.
 * @param type The type of the label.
 */
void handleLabel(char *label, Node **symbolTableHead, int type);

/**
 * @brief This function handles a constant.
 * 
 * This function adds the constant to the symbol table with the constant value.
 * 
 * @param parsedLine The line parsed by the space.
 * @param symbolTableHead A pointer to the head of the symbol table.
 * @param wordAmount The amount of words in the line.
 */
void handleConstant(char **parsedLine, Node **symbolTableHead, int wordAmount);

/**
 * @brief This function checks the type of the line.
 * 
 * This function checks the type of the line, converts it to binary and returns it.
 * 
 * @param line The line cleaned to check.
 * @param originalLine The original line.
 * @return The binary code of the line.
 */
char *checkLineType(char *line, char *originalLine);

/**
 * @brief This function gets an operand and returns it in binary.
 * 
 * This function gets any type of operand, and all of the information needed to convert it to binary.
 * 
 * @param operand The operand to convert.
 * @param symbolTableHead A pointer to the head of the symbol table.
 * @param addressingMethod The addressing method of the operand.
 * @param isConstant A flag to check if the operand is a constant.
 * @param isSource A flag to check if the operand is a source operand.
 * @param hasLabel A flag to check if the operand has a label.
 * @return The binary code of the operand.
 */
char *operandHandling(char *operand, Node **symbolTableHead, int addressingMethod, int isConstant, int isSource, int *hasLabel);

/**
 * @brief This function handles the special case of two registers.
 * 
 * This function gets two registers and returns them in binary.
 * 
 * @param soruceRegister The source register.
 * @param destinationRegister The destination register.
 * @return The binary code of the two registers.
 */
char *handleTwoRegisters(char *soruceRegister, char *destinationRegister);

/**
 * @brief This function executes the first pass algorithm and then starts the second pass.
 * 
 * @param file - The input file to read from.
 * @param outputFileName - The name of the output file.
 */
void executeFirstPass(char *file, char **outputFileName);

#endif
