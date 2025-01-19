CC := gcc
CFLAGS := -Wall -Wextra -Iinclude -g

SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin
TEST_DIR := tests

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(filter-out $(SRC_DIR)/main.c, $(SRCS)))
TARGET := $(BIN_DIR)/interpreter

all: $(TARGET)

$(TARGET): $(OBJS) $(BUILD_DIR)/main.o | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(BUILD_DIR):
	mkdir -p $@

run: $(TARGET)
	$(TARGET)

test: $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/test_runner $(TEST_DIR)/test_runner.c $(OBJS) $(TEST_DIR)/lexer_test.h $(TEST_DIR)/lexer_test.c $(TEST_DIR)/test_util.h $(TEST_DIR)/test_util.c $(TEST_DIR)/parser_test.h $(TEST_DIR)/parser_test.c
	$(BIN_DIR)/test_runner

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all test clean
