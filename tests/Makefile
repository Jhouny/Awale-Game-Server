# Tests Makefile (follows project's Makefile structure)
CC = gcc
CFLAGS = -Wall -Wextra -g -std=c17 -I../shared -D_POSIX_C_SOURCE=200112L
LDFLAGS = -lcriterion -pthread

BUILD_DIR = build
BIN_DIR = bin

DATABASE_DIR = database
CLIENT_DIR = client
SERVER_DIR = server
DBMS_DIR = dbms

DATABASE_SRCS = $(wildcard $(DATABASE_DIR)/*.c)
CLIENT_SRCS = $(wildcard $(CLIENT_DIR)/*.c)
SERVER_SRCS = $(wildcard $(SERVER_DIR)/*.c)
DBMS_SRCS = $(wildcard $(DBMS_DIR)/*.c)

DATABASE_OBJS = $(patsubst $(DATABASE_DIR)/%.c,$(BUILD_DIR)/$(DATABASE_DIR)/%.o,$(DATABASE_SRCS))
CLIENT_OBJS   = $(patsubst $(CLIENT_DIR)/%.c,$(BUILD_DIR)/$(CLIENT_DIR)/%.o,$(CLIENT_SRCS))
SERVER_OBJS   = $(patsubst $(SERVER_DIR)/%.c,$(BUILD_DIR)/$(SERVER_DIR)/%.o,$(SERVER_SRCS))
DBMS_OBJS     = $(patsubst $(DBMS_DIR)/%.c,$(BUILD_DIR)/$(DBMS_DIR)/%.o,$(DBMS_SRCS))

.PHONY: all database client server dbms clean
all: database client server dbms

# Individual test targets
database: $(BIN_DIR)/test_database
client:   $(BIN_DIR)/test_client
server:   $(BIN_DIR)/test_server
dbms:     $(BIN_DIR)/test_dbms

# Link rules
$(BIN_DIR)/test_database: $(DATABASE_OBJS) | $(BIN_DIR)
	@echo "Linking tests/database/test_database..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BIN_DIR)/test_client: $(CLIENT_OBJS) | $(BIN_DIR)
	@echo "Linking tests/client/test_client..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BIN_DIR)/test_server: $(SERVER_OBJS) | $(BIN_DIR)
	@echo "Linking tests/server/test_server..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BIN_DIR)/test_dbms: $(DBMS_OBJS) | $(BIN_DIR)
	@echo "Linking tests/dbms/test_dbms..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile rules
$(BUILD_DIR)/$(DATABASE_DIR)/%.o: $(DATABASE_DIR)/%.c ../shared/common.h | $(BUILD_DIR)/$(DATABASE_DIR)
	@echo "Compiling (tests/database) $<..."
	$(CC) $(CFLAGS) -I../database -c $< -o $@

$(BUILD_DIR)/$(CLIENT_DIR)/%.o: $(CLIENT_DIR)/%.c ../shared/common.h | $(BUILD_DIR)/$(CLIENT_DIR)
	@echo "Compiling (tests/client) $<..."
	$(CC) $(CFLAGS) -I../client -c $< -o $@

$(BUILD_DIR)/$(SERVER_DIR)/%.o: $(SERVER_DIR)/%.c ../shared/common.h | $(BUILD_DIR)/$(SERVER_DIR)
	@echo "Compiling (tests/server) $<..."
	$(CC) $(CFLAGS) -I../server -c $< -o $@

$(BUILD_DIR)/$(DBMS_DIR)/%.o: $(DBMS_DIR)/%.c ../shared/common.h | $(BUILD_DIR)/$(DBMS_DIR)
	@echo "Compiling (tests/dbms) $<..."
	$(CC) $(CFLAGS) -I../dbms -c $< -o $@
# Ensure build directories exist
$(BUILD_DIR)/$(DATABASE_DIR):
	mkdir -p $@

$(BUILD_DIR)/$(CLIENT_DIR):
	mkdir -p $@

$(BUILD_DIR)/$(SERVER_DIR):
	mkdir -p $@

$(BUILD_DIR)/$(DBMS_DIR):
	mkdir -p $@

$(BIN_DIR):
	mkdir -p $@

# Run all tests
test: all
	@for p in database client server dbms; do \
		$(BIN_DIR)/test_$$p; \
	done

# Clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)