#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "../Headers/errors.h"

/*
The array specifies the error codes and their corresponding messages.
*/
Error errors[] = {
    {ERROR_CODE_0, "No error"},
    {ERROR_CODE_1, "The file name is too long"},
    {ERROR_CODE_2, "Macro has more than one definition"},
    {ERROR_CODE_3, "Illegal name for macro"},
    {ERROR_CODE_4, "endmcro has extra text afterwards"},
    {ERROR_CODE_5, "Failed to copy file during macro expansion"},
    {ERROR_CODE_6, "Macro expansion in an .as file failed"},
    {ERROR_CODE_7, "Macro call before declaration"},
    {ERROR_CODE_8, ".as files name is missing"},
    {ERROR_CODE_9, ".am file is missing"},
    {ERROR_CODE_10, "Failed to dynamically allocate memory"},
    {ERROR_CODE_11, "The provided file name doesn't exist"},
    {ERROR_CODE_12, "The line is too long"},
    {ERROR_CODE_13, "Failed to open new file for writing"},
    {ERROR_CODE_14, "Failed to open file for reading"},
    {ERROR_CODE_15, "Macro without name"},
    {ERROR_CODE_16, "Extra text after macro name definition"},
    {ERROR_CODE_17, "Setting a position in a file failed"},
    {ERROR_CODE_18, "Illegal opcode"},
    {ERROR_CODE_19, "Line containing extra text"},
    {ERROR_CODE_20, "Missing argument"},
    {ERROR_CODE_21, "Illegal argument"},
    {ERROR_CODE_22, "Missing comma between arguments"},
    {ERROR_CODE_23, "Label not defined in the assembly file"},
    {ERROR_CODE_24, "Illegal label after .entry"},
    {ERROR_CODE_25, "Illegal character"},
    {ERROR_CODE_26, "Comma in wrong place"},
    {ERROR_CODE_27, "Too many commas"},
    {ERROR_CODE_28, "Illegal comma"},
    {ERROR_CODE_29, "Missing comma between numbers"},
    {ERROR_CODE_30, "Illegal label declaration"},
    {ERROR_CODE_31, "Missing ':' after label declaration"},
    {ERROR_CODE_32, "Illegal register name. The register name should be r0-r7"},
    {ERROR_CODE_33, "Illegal comma before opcode"},
    {ERROR_CODE_34, "Data line without '.' before directive"},
    {ERROR_CODE_35, "Instruction '.data' line contains non-numeric characters"},
    {ERROR_CODE_36, "Comma after last number in '.data' line"},
    {ERROR_CODE_37, "Missing '\"' after '.string' directive"},
    {ERROR_CODE_38, "Extra text after '.string' directive"},
    {ERROR_CODE_39, "IC too big for word CPU word length"},
    {ERROR_CODE_40, "Label definition repeates more than once"},
    {ERROR_CODE_41, "Label defined as .extern and defined in the file"},
    {ERROR_CODE_42, "Input number in .data line is out of range"},
    {ERROR_CODE_43, "Illegal data line directive"},
    {ERROR_CODE_44, "Instruction '.data' line contains illegal chars or syntax error"}
};

void printIntError(int errorID)
{
    /* Print the error message to the console with the error code */
    printf("ERROR: ID: %d | %s\n", errorID, errors[errorID].errorString);
}

void printExtError(int errorID, location file)
{
    /* Print the error message to the console with the error code and the file name */
    printf("ERROR: ID: %d | in %s at line %d | there is error %s\n", errorID, file.fileName, file.line, errors[errorID].errorString);
}