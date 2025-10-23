# --- Compiler Configuration ---
CC = gcc
CFLAGS = -Wall -Wextra -std=c17 -I./src

# --- Project Configuration ---
TARGET = awale  # The name of the final executable
# List all source files (.c files) in the 'src' directory
# Assuming all .h files listed (Client, GameEngine, Objects, Server, CSVHandler)
# have corresponding .c files that need to be compiled.
SRCS = src/Main.c src/GameEngine.c src/Objects.c src/Set.c
# Automatically generate the list of object files (.o) from the source files
OBJS = $(SRCS:.c=.o)

# --- Build Rules ---

# Default target: builds the executable
.PHONY: all
all: $(TARGET)

# Rule to link the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# Pattern rule for compiling a .c file into a .o file
# This rule handles all files in the SRCS list automatically.
# $< is the dependency (the .c file)
# $@ is the target (the .o file)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# --- Utility Targets ---

# Clean target: removes all generated files
.PHONY: clean
clean:
	# Remove all object files and the final executable
	rm -f $(OBJS) $(TARGET)
	# Note: If your .o files are placed in a different directory, adjust the path here.
