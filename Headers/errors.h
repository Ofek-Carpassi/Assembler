#ifndef ERRORS_H
#define ERRORS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globalVars.h"

/* Used to represent an error with an ID and a string. */
typedef struct Error {
    int errorID;        /* The ID of the error. */
    char *errorString;  /* The string of the error. */
} Error;

/* The error codes. */
typedef enum ERROR_CODES {
    ERROR_CODE_0 = 0,
    ERROR_CODE_1 = 1,
    ERROR_CODE_2 = 2
}

/**
* This function prints an error message to the stdout that is related to an error in the source file.
*
* @param errorID The ID of the error.
* @param file a pointer to a struct containing the source file and the line info.
*/
void printExtError(int errorID, location file);

/**
* This function prints an error message to the stdout that is related to an internal process.
*
* @param errorID The ID of the error.
*/
void printIntError(int errorID);

#endif