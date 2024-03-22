#ifndef ERRORS_H
#define ERRORS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globalVariables.h"

/* Used to represent an error with an ID and a string. */
typedef struct Error {
    int errorID;        /* The ID of the error. */
    char *errorString;  /* The string of the error. */
} Error;

/* The error codes. */
typedef enum ERROR_CODES {
    /* The following errors are macro related or general */
    ERROR_CODE_0 = 0,
    ERROR_CODE_1,
    ERROR_CODE_2,
    ERROR_CODE_3,
    ERROR_CODE_4,
    ERROR_CODE_5,
    ERROR_CODE_6,
    ERROR_CODE_7,
    ERROR_CODE_8,
    ERROR_CODE_9,
    ERROR_CODE_10,
    ERROR_CODE_11,
    ERROR_CODE_12,
    ERROR_CODE_13,
    ERROR_CODE_14,
    ERROR_CODE_15,
    ERROR_CODE_16,
    ERROR_CODE_17,
    ERROR_CODE_18,
    ERROR_CODE_19,
    ERROR_CODE_20,
    ERROR_CODE_21,
    ERROR_CODE_22,
    ERROR_CODE_23,
    ERROR_CODE_24,
    ERROR_CODE_25,
    ERROR_CODE_26,
    ERROR_CODE_27,
    ERROR_CODE_28,
    ERROR_CODE_29,
    ERROR_CODE_30,
    ERROR_CODE_31,
    ERROR_CODE_32,
    ERROR_CODE_33,
    ERROR_CODE_34,
    ERROR_CODE_35,
    ERROR_CODE_36,
    ERROR_CODE_37,
    ERROR_CODE_38,
    ERROR_CODE_39,
    ERROR_CODE_40,
    ERROR_CODE_41,
    ERROR_CODE_42,
    ERROR_CODE_43,
    ERROR_CODE_44,
    ERROR_CODE_45,
    ERROR_CODE_46,
    ERROR_CODE_47
} ERROR_CODES;

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