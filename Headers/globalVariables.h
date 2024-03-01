#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

#define MAX_LINE_LENGTH 81

#define MAX_LABEL_LENGTH 31

#define IC_INIT_VALUE 100

#define BIG_NUMBER_CONST 10000

#define OPCODES_COUNT 16

#define REG_COUNT 8

#define WORD_LENGTH 12

#define ARE_BITS 2

#define RELOCATABLE_VALUE 2

#define EXTERNAL_VALUE 1

#define INSTRUCTIONS_COUNT 4

#define SIX_BITS_MAX_VALUE 63

#define SIX_BITS 6

#define NUM_SEXTETES 2

#define MAX_NUM ((1 << (WORD_LENGTH - 1)) - 1)

#define MIN_NUM (-(1 << (WORD_LENGTH - 1)))

typedef struct location {
    char *fileName;
    int line;
} location;

typedef struct lineData {
    char *fileName;
    short number;
    char *data;
} lineData;

#endif