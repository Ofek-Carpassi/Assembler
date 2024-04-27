#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdio.h>

/**
 * @brief This function is used to clean a line from white spaces.
 * 
 * @param line - a string containing the line to clean.
 * @return - a string containing the cleaned line.
 */
char *cleanLine(char *line);

/**
 * @brief This function is used to parse a line.
 * 
 * This function will parse a line and return an array of strings containing the words in the line.
 * 
 * @param line - a string containing the line to parse.
 * @param num_words - the number of words in the line.
 * @param loc - a pointer to the location of the line.
 * @param noErrors - a pointer to the number of errors.
 * @return - an array of strings containing the words in the line.
 */
char** parseLine(char* line, int num_words, location *loc, int *noErrors);

/**
 * @brief This function is used to check if a string is a number.
 * 
 * This function will check if a string is a number and return 1 if it is, 0 otherwise.
 * 
 * @param str - a string to check if it is a number.
 * @return - 1 if the string is a number, 0 otherwise.
 */
int isNumber(char *str);

/**
 * @brief This function is used to count the number of words in a line.
 * 
 * This function will count the number of words in a line and return the count.
 * 
 * @param line - a string containing the line to count the words in.
 * @return - the number of words in the line.
 */
int countWords(char *line);

/**
 * @brief This function is used to convert an integer to binary.
 * 
 * This function will convert an integer to binary and return the binary representation.
 * 
 * @param num - an integer to convert to binary.
 * @param bits - the number of bits to convert the integer to.
 * @return - a string containing the binary representation of the integer.
 */
char *intToBinary(int num, int bits);

/**
 * @brief This function is used to get the addressing method of an operand.
 * 
 * This function will get the addressing method of an operand and return the operand without the addressing method.
 * 
 * @param operand - a string containing the operand to get the addressing method of.
 * @param symbolTable - a pointer to the symbol table.
 * @param addressingMethod - a pointer to the addressing method of the operand.
 * @return - a string containing the addressing method in binary.
 */
char *getAddressingMethod(char *operand, Node *symbolTable, int *addressingMethod);

/**
 * @brief This function is used to remove commas from a string.
 * 
 * This function will remove commas from a string and return the string without commas.
 * 
 * @param line - a string containing the line to remove commas from.
 * @return - a string containing the line without commas.
 */
char *removeCommas(char *line);

/**
 * @brief This function is used to check if all the commas in a line are legal.
 * 
 * @param line - a string containing the line to check the commas in.
 * @param noErrors - a pointer to the number of errors.
 * @param loc - a pointer to the location of the line.
 * @param parsedLine - an array of strings containing the words in the line.
 * @param wordAmount - the number of words in the line.
 */
void isLegalCommas(char *line, int *noErrors, location *loc, char **parsedLine, int wordAmount);

#endif
