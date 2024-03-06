/*
Algorithm:
1. IC (Instruction Counter) and DC (Data Counter) are initialized to 0.
2. Read a line from the input file - if the input has ended, go to step 17.
3. If the line is empty or a comment, go to step 2.
4. Is the line a constant declaration (starts with .define)? If not go to step 6.
5. Add the constant to the symbol table with mdefine as its type and its value as its value. Go to step 2.
6. Is the line a label declaration? If not go to step 8.
7. Turn flag "isLabel" on
8. Is it an instruction to save data (.data or .string)? If not go to step 11.
9. If there's a label declaration, add it to the symbol table with data type, its value is DC.
10. Find the type of data, code them to the memory and update DC by the length of the data.
    If the data is a .data and it has a value that is a constant, make sure the constant is in the symbol table with mdefine. Go to step 2.
11. Is it an instruction to save code (.entry or .extern)? If not go to step 13.
12. Is it a .extern? If so, add each label that shows as an operand to the symbol table without a value and with type external. Go to step 2.
13. Is it a label declaration? If so, add it to the symbol table with code type and its value is IC+100.
14. Find the operation name in the operation table and code it to the memory.
15. Analyze the operands structure and calculate L (amount of words the instruction takes). 
    Build the binary code of the first word of the instruction and code it to the memory. 
    If there's a second word, code it to the memory. Update IC by L.
16. Update IC by L. Go to step 2.
17. The input file was read, if there were any errors on the first pass, stop here.
18. Update the symbol table of each data type with the value of IC + 100.
19. Start the second pass.
*/