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

#endif