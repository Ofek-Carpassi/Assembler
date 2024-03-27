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
    
    char *outputFileName = malloc(sizeof(char) * 9);
    //executePreAssembler("test.txt", &outputFileName);
    executeFirstPass("test.txt", &outputFileName);
    printf("%s\n", outputFileName);
    
    // Node *head = NULL;
    
    // char *res = handleInstruction("cmp r3 #2", &head);
    
    // printf("%s\n", res);

    /*
    Operand One: r7, Operand Two: r6
    The current binary line is: 00000011
    expect:
    00000011111100
    00000001111100
    */
   
    return 0;
}

// gcc Source/dataStructers.c Source/errors.c Source/preAssembler.c Source/testing.c Source/utilities.c Source/firstPass.c -o test