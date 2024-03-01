#ifndef CODE_CONVERTOR_H
#define CODE_CONVERTOR_H

#include "translator.h"
#include "globalVars.h"

#define SHIFT_SOURCE 9 /* number of shifts needed to get the source operand */
#define SHIFT_OPCODE 5 /* number of shifts needed to get the opcode */
#define SHIFT_DEST 2   /* number of shifts needed to get the destination operand */

#define SHIFT_REGISTER_SOURCE 7 /* number of shifts needed to get the source register */
#define SHIFT_REGISTER_DEST 2   /* number of shifts needed to get the destination register */

/* A,R,E for addressing methods */
#define ABSOLUTE_ADDRESSING 1
#define RELOCATABLE_ADDRESSING 3
#define EXTERNAL_ADDRESSING 5

/* Value to indicate that both registers have been with to avoid dealing twice with destination register */
#define REGS_DOUBLE_VALUES 1000000

/* Memory limit */
#define IC_MAX 1023

/**
* This struct stores the converted code line, 
* including the unsigned short number representation of the line,
* the label associated with the line, and the line number in the source file.
*/
typedef struct codeConver {
    unsigned short shortNum; /* The numberical representation of the line */
    char *label;             /* Optional label associated with the line */
    int lineNumSrc;          /* The line number in the source file */
} codeConver;

/**
* This function takes a pointer to a codeConver array and increase its memory allocation
* by reallocating the array with a larger size indicated by the counter.
* If the reallocation is successful, the function returns 1. If the reallocation fails, 
* an internal error message is printed, the memory is freed, and the function returns 0.
*
* @param code - A pointer to a codeConver array
* @param counter - The number of elements to add to the array
* @return 1 if the reallocation is successful, 0 otherwise
*/
int increaseMemory(codeConver **code, int counter);

/**
* This function takes a CommandParts struct and converts the src and the dst operands,
* as well as the opcode, into binary representation according to the addressing method.
* The converted values are then combined into a single unsigned short number and returned.
*
* @param command - A pointer to a CommandParts struct containing all the information about the command
* @return The unsigned short number representation of the command
*/
unsigned short convertCommandToCode(CommandParts *command);

int addMachineCodeData(codeConver **code, instParts *inst, int *DC, location am_file);