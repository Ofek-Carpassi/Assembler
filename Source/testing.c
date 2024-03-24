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
    /*
    char *outputFileName = malloc(sizeof(char) * 9);
    executePreAssembler("test.txt", &outputFileName);
    executeFirstPass("test.txt", &outputFileName);
    printf("%s\n", outputFileName);
    */
    Node *head = NULL;
    char *bL = "00000001";
    // mov r1, r4
    char *res = handleTwoOperands("r3", "#2", &head, bL);
    printf("%s\n", res);
   
    return 0;
}

// gcc Source/dataStructers.c Source/errors.c Source/preAssembler.c Source/testing.c Source/utilities.c Source/firstPass.c -o test