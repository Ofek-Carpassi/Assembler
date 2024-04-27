# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -ansi -pedantic

# Source files
SOURCES = Source/dataStructures.c Source/Errors.c Source/firstPass.c Source/preAssembler.c Source/Assembler.c Source/utilities.c Source/secondPass.c 

# Header files
HEADERS = Header/dataStructures.h Header/errors.h Header/firstPass.h Header/globalVariables.h Header/preAssembler.h Header/utilities.h Header/secondPass.h

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Target executable
TARGET = assembler

# Default target
all: $(TARGET)

# Build the target
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) =.-o $(TARGET) $(OBJECTS)

# Build object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up object files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Run testing
run:
	./$(TARGET)
