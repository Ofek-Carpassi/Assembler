# Compiler
CC = gcc
# Compiler flags
CFLAGS = -Wall -Wextra -IHeaders

# Source files directory
SRCDIR = Source
# Header files directory
INCDIR = Headers
# Object files directory
OBJDIR = obj

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.c)
# Object files
OBJECTS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))

# Executable name
EXECUTABLE = my_program

# Phony targets
.PHONY: all clean

# Default target
all: $(EXECUTABLE)

# Linking the object files to create the executable
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

# Compiling each source file into object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create the object directory if it does not exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean rule
clean:
	rm -rf $(OBJDIR) $(EXECUTABLE)
