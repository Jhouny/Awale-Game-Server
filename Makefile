# --- Compiler Configuration ---
CC = gcc
CFLAGS = -Wall -Wextra -std=c17 -I./src

# --- Build Directory ---
BUILD_DIR = build


# --- Project Configuration ---
TARGET = awale  # The name of the final executable
SRCS = $(wildcard src/*.c)
# Place object files in build directory
OBJS = $(SRCS:src/%.c=$(BUILD_DIR)/%.o)


# --- Build Rules ---

# Default target: builds the executable
.PHONY: all
all: $(BUILD_DIR)/$(TARGET)

# Rule to link the final executable
$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

# Pattern rule for compiling a .c file into a .o file
# $< is the dependency (the .c file)
# $@ is the target (the .o file)
$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)


# --- Utility Targets ---

# Clean target: removes all generated files
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
