#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

#define MAX_LINE_LENGTH 81

#define OPCODES_COUNT 16

#define BITS_AMOUNT 14

enum lineType {
    ERROR,
    COMMENT,
    CONSTANT,
    LABEL,
    DATA,
    STRING,
    ENTRY,
    EXTERN,
    INSTRUCTION
};

#endif