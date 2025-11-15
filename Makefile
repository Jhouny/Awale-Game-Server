# --- Compiler Configuration ---
CC = gcc
CFLAGS = -Wall -Wextra -g -std=c17 -Ishared -D_POSIX_C_SOURCE=200112L

# --- Build Directory ---
BUILD_DIR = build
BIN_DIR = bin
SHARED_DIR = shared

CLIENT_DIR = client
SERVER_DIR = server
DATABASE_DIR = database
DBMS_DIR = dbms

# --- Project Configuration ---
CLIENT_SRCS = $(wildcard $(CLIENT_DIR)/*.c)
SERVER_SRCS = $(wildcard $(SERVER_DIR)/*.c)
DATABASE_SRCS = $(wildcard $(DATABASE_DIR)/*.c)
DBMS_SRCS = $(wildcard $(DBMS_DIR)/*.c)
SHARED_SRCS = $(wildcard $(SHARED_DIR)/*.c)

CLIENT_OBJS = $(patsubst client/%.c,$(BUILD_DIR)/client/%.o,$(CLIENT_SRCS)) \
	$(patsubst $(SHARED_DIR)/%.c,$(BUILD_DIR)/shared/%.o,$(SHARED_SRCS))

SERVER_OBJS = $(patsubst server/%.c,$(BUILD_DIR)/server/%.o,$(SERVER_SRCS)) \
	$(patsubst $(SHARED_DIR)/%.c,$(BUILD_DIR)/shared/%.o,$(SHARED_SRCS))

DATABASE_OBJS = $(patsubst database/%.c,$(BUILD_DIR)/database/%.o,$(DATABASE_SRCS)) \
	$(patsubst $(SHARED_DIR)/%.c,$(BUILD_DIR)/shared/%.o,$(SHARED_SRCS))

DBMS_OBJS = $(patsubst dbms/%.c,$(BUILD_DIR)/dbms/%.o,$(DBMS_SRCS)) \
	$(patsubst $(SHARED_DIR)/%.c,$(BUILD_DIR)/shared/%.o,$(SHARED_SRCS))

# --- Executable Names ---
CLIENT_EXEC = $(BIN_DIR)/client
SERVER_EXEC = $(BIN_DIR)/server
DATABASE_EXEC = $(BIN_DIR)/database
DBMS_EXEC = $(BIN_DIR)/dbms

# --- Build Rules ---

# Default target: builds the executable
.PHONY: all client server database dbms
all: $(CLIENT_EXEC) $(SERVER_EXEC) $(DATABASE_EXEC) $(DBMS_EXEC)

client: $(CLIENT_EXEC)

server: $(SERVER_EXEC)

database: $(DATABASE_EXEC)

dbms: $(DBMS_EXEC)

# Rule to link the client executable
$(CLIENT_EXEC): $(CLIENT_OBJS) | $(BIN_DIR)
	@echo "Linking client..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to link the server executable
$(SERVER_EXEC): $(SERVER_OBJS) | $(BIN_DIR)
	@echo "Linking server..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to link the database executable
$(DATABASE_EXEC): $(DATABASE_OBJS) | $(BIN_DIR)
	@echo "Linking database..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to link the dbms executable
$(DBMS_EXEC): $(DBMS_OBJS) | $(BIN_DIR)
	@echo "Linking dbms..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Pattern rule for compiling a .c file into a .o file
# $< is the dependency (the .c file)
# $@ is the target (the .o file)
$(BUILD_DIR)/shared/%.o: $(SHARED_DIR)/%.c $(SHARED_DIR)/common.h | $(BUILD_DIR)/shared
	@echo "Compiling (shared) $<..."
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/server/%.o: $(SERVER_DIR)/%.c $(SHARED_DIR)/common.h | $(BUILD_DIR)/server
	@echo "Compiling (server) $<..."
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/client/%.o: $(CLIENT_DIR)/%.c $(SHARED_DIR)/common.h | $(BUILD_DIR)/client
	@echo "Compiling (client) $<..."
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/database/%.o: $(DATABASE_DIR)/%.c $(SHARED_DIR)/common.h | $(BUILD_DIR)/database
	@echo "Compiling (database) $<..."
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/dbms/%.o: $(DBMS_DIR)/%.c $(SHARED_DIR)/common.h | $(BUILD_DIR)/dbms
	@echo "Compiling (dbms) $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure build directory exists
$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/shared:
	mkdir -p $@

$(BUILD_DIR)/server:
	mkdir -p $@

$(BUILD_DIR)/client:
	mkdir -p $@

$(BUILD_DIR)/database:
	mkdir -p $@

$(BUILD_DIR)/dbms:
	mkdir -p $@

$(BIN_DIR):
	mkdir -p $@

# --- Utility Targets ---

# Clean target: removes all generated files
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
