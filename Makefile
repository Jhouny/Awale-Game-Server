# --- Compiler Configuration ---
CC = gcc
CFLAGS = -Wall -Wextra -g -std=c17 -Ishared -D_POSIX_C_SOURCE=200112L

# --- Build Directory ---
BUILD_DIR = build
BIN_DIR = bin
SHARED_DIR = shared

CLIENT_DIR = client
SERVER_DIR = server

# --- Project Configuration ---
CLIENT_SRCS = $(wildcard $(CLIENT_DIR)/*.c)
SERVER_SRCS = $(wildcard $(SERVER_DIR)/*.c)
SHARED_SRCS = $(wildcard $(SHARED_DIR)/*.c)

CLIENT_OBJS = $(patsubst client/%.c,$(BUILD_DIR)/%.o,$(CLIENT_SRCS)) \
	$(patsubst $(SHARED_DIR)/%.c,$(BUILD_DIR)/%.o,$(SHARED_SRCS))

SERVER_OBJS = $(patsubst server/%.c,$(BUILD_DIR)/%.o,$(SERVER_SRCS)) \
	$(patsubst $(SHARED_DIR)/%.c,$(BUILD_DIR)/%.o,$(SHARED_SRCS))

CLIENT_EXEC = $(BIN_DIR)/client
SERVER_EXEC = $(BIN_DIR)/server

# --- Build Rules ---

# Default target: builds the executable
.PHONY: all
all: $(CLIENT_EXEC) $(SERVER_EXEC)

# Rule to link the client executable
$(CLIENT_EXEC): $(CLIENT_OBJS) | $(BIN_DIR)
	@echo "Linking client..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to link the server executable
$(SERVER_EXEC): $(SERVER_OBJS) | $(BIN_DIR)
	@echo "Linking server..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Pattern rule for compiling a .c file into a .o file
# $< is the dependency (the .c file)
# $@ is the target (the .o file)
$(BUILD_DIR)/%.o: $(SHARED_DIR)/%.c $(SHARED_DIR)/common.h | $(BUILD_DIR)
	@echo "Compiling (shared) $<..."
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SERVER_DIR)/%.c $(SHARED_DIR)/common.h | $(BUILD_DIR)
	@echo "Compiling (server) $<..."
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(CLIENT_DIR)/%.c $(SHARED_DIR)/common.h | $(BUILD_DIR)
	@echo "Compiling (client) $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure build directory exists
$(BUILD_DIR):
	mkdir -p $@

$(BIN_DIR):
	mkdir -p $@


# --- Utility Targets ---

# Clean target: removes all generated files
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
