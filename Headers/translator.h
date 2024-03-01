#ifndef TRANSLATOR_H
#define TRANSLATOR_H

/* Represents an operation code with its opcode and the number of arguments it takes */
typedef struct opCode {
    char *code;     /* The opcode corresponding to the command */
    int argAmount;  /* The number of arguments the opcode takes */
} opCode;

typedef struct commandParts {
    char *label;    /* The label associated with the command */
    int opcode;     /* The opcode of the command */
    char *src;      /* The source operand of the command */
    char *dst;      /* The destination operand of the command */
} CommandParts;

typedef struct instructionParts {    
    char *label;    /* The label associated with the instruction */
    short *numbers; /* An array of the shorts that represent the instruction data */
    int length;     /* The number of data in *numbers including '\0' at the end */
    char *argLabel; /* The label associated with the argument */
    int isExternal; /* A flag to indicate if the instruction is external */
} instructionParts;

/**
* This function reads an entry or extern directive from a string. It tokenizes the string and extracts
* all the necessary information, and creates a new instructionParts struct to store the data.
* The function handles memory allocation for the new struct and returns a pointer to it.
* It also updates the errorCode if an error is encountered.
*
* @param string - A string containing the directive to read
* @param errorCode - A pointer to an integer to update if an error is encountered
* @return A pointer to an instructionParts struct containing the data from the directive
*/
instructionParts *readEntryExtern(char *string, int *errorCode);

/**
* This function reads every line in the file and if one line is longer than the `MAX_LENGTH_LINE`,
* it print an external error message using `ERROR_CODE_30` and sets a flag to indicate that the line is too long.
*
* @param file - A string containing the file name to read from
* @return 1 if the line is too long, 0 otherwise
*/
int lineTooLong(char *file);

/**
* This function compares a string to a list of all known instructions.
* If there's a match, the function returns 1. Otherwise, it returns 0.
*
* @param string - A string containing the instruction to compare
* @return 1 if the string is an instruction, 0 otherwise
*/
int isInstruction(char *string);

/**
* This function compares a string with a list of opcodes.
* If there's a match, the function returns the index of the matching opcode. Otherwise, it returns -1.
*
* @param string - A string containing the opcode to compare
* @return The index of the matching opcode, or -1 if no match is found
*/
int whichOPCODE(char *string);

/**
 * This function compares a string with a list of registers.
 * If there's a match, the function returns the index of the matching register. Otherwise, it returns -1.
 * 
 * @param string a string containing the register to compare
 * @return the index of the matching register, or -1 if no match is found
 */
int whichRegister(char *string);

/**
 * This function validates the given string via the following rules:
 * 1. The string isn't NULL
 * 2. The first char is a letter
 * 3. The length is less or equal to 'MAX_LENGTH_LABEL'
 * 4. The label isn't a register or an opcode (not found in the opcodes array nor in the registers array)
 * 5. After the first char, the label contains only letters and digits
 * 6. The label need to end with a colon, followed by the null terminator
 * 
 * @param string a string containing the label to validate
 * @return 1 if the label is valid, 0 otherwise
 */
int isLebalDeclarationLegal(char *string);

/**
 * This function checks if the provided string is a legal label according to the following rules:
 * 1. The string isn't NULL
 * 2. The first char is a letter
 * 3. The length is less or equal to 'MAX_LENGTH_LABEL'
 * 4. The label doesn't contain a register or an opcode (not found in the opcodes array nor in the registers array)
 * 5. After the first char, the label contains only letters and digits until the null terminator
 * 
 * @param string a string containing the label to validate
 * @return 1 if the label is valid, 0 otherwise
 */
int isLegalLabel(char *string)

/**
 * This function uses the standard library function `strtok` with the '\n' char as the delimiter.
 * If there is more text after the '\n' char, the function will return 1. Otherwise, it will return 0.
 * 
 * @return 1 if there is extra text, 0 otherwise
 */
int isThereExtraText();

/**
* This function searches for the first occurrence of a colon in a string and adds a space after it.
*
* @param string - A pointer to a string to modify
* @param errorCode - Pointer to integer variable to store error code
* @return 1 if the space is added successfully, 0 otherwise
*/
int addSpaceAfterColon(char **string, int *errorCode);

/**
* This function uses the 'whichRegister' func to check if the string is a valid register and 'isLegalLabel' to check if the string is a valid label.
* If the string is a valid register or label, the function returns 1. Otherwise, it returns 0.
*
* @param string - A string containing the register or label to check
* @return 1 if the string is a valid register or label, 0 otherwise
*/
int isRegisterOrLabel(char *string);

/**
* This function checks for error related to the register usage in a string.
* It verifies if the string contains any whitespace chars using the 'hasWhiteSpace' func.
* If the string contains whitespace it sets the errorCode to 'ERROR_CODE_35', indicating an error due to whitespace in the register.
* It also check if the string is a valid register
*
* @param string - A string containing the register to check
* @param errorCode - A pointer to an integer to update if an error is encountered
*/
void checkRegisterErrors(char *string, int *errorCode);

/**
* This function uses 'strtol' to convert the string to a number.
* If the entire string can be converted to a number, the function returns 1. Otherwise, it returns 0.
*
* @param string - A string to check if it's a number
* @return 1 if the string is a number, 0 otherwise
*/
int isNumber(char *string);

/**
* This function uses 'isRegisterOrLabel' to check if the string is a valid register or label and 'isNumber' to check if the string is a valid number.
* If the string is a valid register, label or number, the function returns 1. Otherwise, it returns 0.
*
* @param string - A string to check if it's a register, label or number
* @return 1 if the string is a register, label or number, 0 otherwise
*/
int isRegisterOrLabelOrNumber(char *string);

/**
int isLegalArgument(char *string, commandParts *command, int *errorCode);