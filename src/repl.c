#include "repl.h"
#include "lexer.h"
#include "parser.h"
#include "util_repr.h"
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <time.h>

#define MAX_INPUT_BUFFER_SIZE 1024
#define PROMPT ">>> "
#define CONTINUOUS_PROMPT "... "
#define ARC_LANG "Arc-lang 0.1.0"

void start_up_info() {
  // prettier-ignore
  // clang-format off
  printf(
    "  ___  ______  _____            _____ _   _ _____ _______________________ _____ _____ ___________ \n"
    " / _ \\ | ___ \\/  __ \\          |_   _| \\ | |_   _|  ___| ___ \\ ___ \\ ___ \\  ___|_   _|  ___| ___ \\\n"
    "/ /_\\ \\| |_/ /| /  \\/  ______    | | |  \\| | | | | |__ | |_/ / |_/ / |_/ / |__   | | | |__ | |_/ /\n"
    "|  _  ||    / | |     |______|   | | | . ` | | | |  __||    /|  __/|    /|  __|  | | |  __||    / \n"
    "| | | || |\\ \\ | \\__/\\           _| |_| |\\  | | | | |___| |\\ \\| |   | |\\ \\| |___  | | | |___| |\\ \\ \n"
    "\\_| |_/\\_| \\_| \\____/           \\___/\\_| \\_/ \\_/ \\____/\\_| \\_\\_|   \\_| \\_\\____/  \\_/ \\____/\\_| \\_|\n"
  );

    time_t now = time(NULL);
    struct tm *tm = localtime(&now); 
    char timer_buffer[64];
    strftime(timer_buffer, 64, "%b %d %Y, %H:%M:%S", tm);

    char platform[130];
    struct utsname uts;
    if (uname(&uts) == 0) {
        snprintf(platform, 130, "%s %s", uts.sysname, uts.release);
    } else {
        snprintf(platform, 130, "unknown");
    }
#ifdef __GNUC__
    printf("%s (%s) [GCC %d.%d.%d] on %s\n", ARC_LANG, timer_buffer, __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__, platform);
#else
    printf("%s (%s) on %s", ARC_LANG, timer_buffer, platform);
#endif
    printf("Type \"help\" for more information and \"exit\" to exit.\n");
}

bool is_incomplete(const char *buffer) {
    int open_parens = 0;
    int open_braces = 0;
    int open_brackets = 0;
    int unclosed_quotes = 0;

    for (size_t i = 0; buffer[i] != '\0'; i++) {
        switch (buffer[i]) {
            case '(': open_parens++; break;
            case ')': open_parens--; break;
            case '{': open_braces++; break;
            case '}': open_braces--; break;
            case '[': open_brackets++; break;
            case ']': open_brackets--; break;
            case '\"': unclosed_quotes ^= 1; break;
        }
    }

    return open_parens > 0 || open_braces > 0 || open_brackets > 0 || unclosed_quotes;
}

bool ends_with_semicolon(const char *buffer, size_t size) {
    for (size_t i = size; i > 0; i--) {
        char c = buffer[i - 1];
        if (c == ';') {
            return true;
        } else if (!isspace(c) && is_incomplete(buffer)) {
            return false;
        }
    }
    return false;
}

void evaluate(const char *buffer, size_t size) { 
    struct lexer *l = lexer_init(buffer, size);
    if (!l) {
      printf("Error initializing lexer\n");
    }

    struct parser *p = parser_init(l);
    if (!p) {
      printf("Error initializing parser\n");
      lexer_free(l);
    }

    struct program *program = parser_parse_program(p);
    if (!program) {
      printf("Error parsing program\n");
      parser_free(p);
    }

    if (parser_has_errors(p)) {
      parser_print_errors(p);
    }

    string_t *str = init_string_t(8);
    t_stmt_repr(program->statements[0], str);

    repr_string_t(str);

    ast_program_free(program);
    parser_free(p);
    free_string_t(str);
}

void repl() {
    start_up_info();
    char input[MAX_INPUT_BUFFER_SIZE];
    char *buffer = NULL;
    size_t buffer_size = 0;

    while (1) {
        printf("%s", buffer_size == 0 ? PROMPT : CONTINUOUS_PROMPT);
        if (!fgets(input, MAX_INPUT_BUFFER_SIZE, stdin)) {
            printf("Error reading input. Exiting.\n");
            shutdown();
            break;
        }

        if (feof(stdin) || (buffer_size == 0 && strncmp(input, "exit\n", 5) == 0)) {
            printf("Goodbye!\n");
            shutdown();
            break;
        }

        size_t input_len = strlen(input);
        buffer = realloc(buffer, buffer_size + input_len + 1);
        if (!buffer) {
            fprintf(stderr, "Memory allocation error. Exiting.\n");
            shutdown();
            break;
        }
        strcpy(buffer + buffer_size, input);
        buffer_size += input_len;

        if (!ends_with_semicolon(buffer, buffer_size) || is_incomplete(buffer)) {
            continue;
        }

        // perform evaluations on the buffer
        evaluate(buffer, buffer_size); 

        free(buffer);
        buffer = NULL;
        buffer_size = 0;
    }
    if (buffer) {
        free(buffer);
        buffer = NULL;
    }
}

void shutdown() { 
    // TODO: free resources && perform cleanups
}

