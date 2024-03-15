#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdio.h>

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
 * @brief This function is used to parse a line.
 * 
 * This function will parse a line and updates an array with the parsed output.
 * 
 * @param line - a string containing the line to parse.
 * @param parsedOutput - an array to update with the parsed output.
 */
void parseLine(char *line, char *parsedOutput[]);

/**
 * @brief This function is used to check if a string is a number.
 * 
 * This function will check if a string is a number and return 1 if it is, 0 otherwise.
 * 
 * @param str - a string to check if it is a number.
 * @return - 1 if the string is a number, 0 otherwise.
 */
int isNumber(const char *str);

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
 * This function will convert an integer to binary and update the binary array with the binary representation.
 * 
 * @param num - an integer to convert to binary.
 * @param binary - an array to update with the binary representation.
 */
void intToBinary(int num, char **binary);

/**
 * @brief This function is used to convert an integer to a binary string.
 * 
 * This function will convert an integer to a binary string and return the binary string.
 * 
 * @param num - an integer to convert to a binary string.
 * @return - a string containing the binary representation of the integer.
 */
char *intToBinaryString(int num);

#endif