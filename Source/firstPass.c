/* Execute the first pass of the assembler. */

#include <stdio.h>

/**
 * @brief This function is the first pass of the assembler.
 *
 * @param inputFile The input file.
 * @param outputFile The output file.
 * @return 0 On successful execution.
*/
int firstPass(FILE *inputFile, FILE *outputFile)
{
    /* Call the check for errors function - in the error-check file. The function will return 0 if there are no errors. */
    /* Call the clean up function - in the cleanup file. (cleans all the white spaces and comments from the input file) */
    /* Call the spread macros function - in the spread-macros file. (replaces all the macros in the input file with their values) */
    /* Call the spread constants function - in the spread-constants file. (replaces all the constants in the input file with their values) */
    /* Return 0 for successful execution. */
}