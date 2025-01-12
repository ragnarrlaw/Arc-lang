CC=gcc
OPTIONS=-Wall
SRC_DIR=src
BIN_DIR=bin
EXEC=$(BIN_DIR)/main.o
MAIN_FLAGS=-DINTERPRETER
MAIN_ENTRY=src/main.c

CFLAGS=-Wall -Wextra -g
TEST_DIR=tests
TEST_FLAGS=-DMODULE_LEXER_TEST
TEST_ENTRY=$(TEST_DIR)/module_test.c

TEST_LINK=\
	src/token/token.h src/token/token.c\
	src/parser/parser.h src/parser/parser.c\
	src/lexer/lexer.h src/lexer/lexer.c\
	tests/lexer/lexer_test.c tests/lexer/lexer_test.h\

test:
	@$(CC) $(TEST_ENTRY) $(CFLAGS) $(TEST_FLAGS) $(TEST_LINK) -o $(BIN_DIR)/test.o
	@./$(BIN_DIR)/test.o

test_clean:
	@rm $(BIN_DIR)/test.o

LINK=\
	src/token/token.h src/token/token.c\
	src/lexer/lexer.h src/lexer/lexer.c\
	src/parser/parser.h src/parser/parser.c\
	src/repl/repl.h src/repl/repl.c\

build:
	@$(CC) $(OPTIONS) $(MAIN_ENTRY) $(LINK) -o $(EXEC) $(MAIN_FLAGS)

run: build
	@./$(EXEC)

clean:
	@rm $(BIN_DIR)/main.o
