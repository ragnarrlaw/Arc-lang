CC := gcc
CFLAGS := -Wall -Wextra -Iinclude -g -static
TEST_FLAGS := $(CFLAGS) -Itests -DTRACE_ON=1

SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin
TEST_DIR := tests

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(filter-out $(SRC_DIR)/main.c, $(SRCS)))
TEST_SRCS := $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS := $(patsubst $(TEST_DIR)/%.c, $(BUILD_DIR)/%.o, $(TEST_SRCS))

TARGET := $(BIN_DIR)/interpreter
TEST_TARGET := $(BIN_DIR)/test_runner

all: $(TARGET)

$(TARGET): $(OBJS) $(BUILD_DIR)/main.o | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(TEST_FLAGS) -c $< -o $@

$(BIN_DIR) $(BUILD_DIR):
	mkdir -p $@

run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGET)
	$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJS) $(OBJS) | $(BIN_DIR)
	$(CC) $(TEST_FLAGS) -o $@ $^

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all test clean

