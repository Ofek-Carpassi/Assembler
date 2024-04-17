#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "../Headers/errors.h"
#include "../Headers/preAssembler.h"
#include "../Headers/firstPass.h"
#include "../Headers/dataStructers.h"
#include "../Headers/utilities.h"

int main()
{   
    char *inputFile = "test.txt";
    char *preAssemblerOutput = (char *)malloc(strlen(inputFile));
    char *firstPassOutput = (char *)malloc(strlen(inputFile));
    executePreAssembler("test.txt", &preAssemblerOutput);
    printf("%s\n", preAssemblerOutput);
    executeFirstPass(preAssemblerOutput, &firstPassOutput);
    printf("%s\n", firstPassOutput);
    /* Check if it works with constants too */
    // Node *head = NULL;
    // addNode(&head, "sz", "mdefine", 2);
    // addNode(&head, "LIST", "data", 100);

    // char **parsedLine = (char **)calloc(8, sizeof(char *));
    // parsedLine[0] = "mov";
    // parsedLine[1] = "r1";
    // parsedLine[2] = "r3";

    // printf("%s", handleInstruction(parsedLine, &head, 3));

    return 0;
}

// gcc Source/dataStructers.c Source/errors.c Source/preAssembler.c Source/testing.c Source/utilities.c Source/firstPass.c -o test