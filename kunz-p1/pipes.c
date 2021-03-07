#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "pipes.h"

#define VERTICAL 0
#define INPUT 1
#define OUTPUT 2

int order_of_pipes[100];

int is_pipe(char **tokens, int token_count) {
    int ret = 0;
    for (int i = 0; i < token_count; i++) {
        if (strcmp(tokens[i], "|") == 0 ||
                strcmp(tokens[i], ">") == 0 ||
                strcmp(tokens[i], "<") == 0) {
            ret++;
        }
    }
    return ret;
}

void generate_order(char **tokens, int token_count) {
    int pipe_index = 0;
    for (int i = 0; i < token_count; i++) {
        if (strcmp(tokens[i], "|") == 0) {
            order_of_pipes[pipe_index] = VERTICAL;
            pipe_index++;
        } else if (strcmp(tokens[i], "<") == 0) {
            order_of_pipes[pipe_index] = INPUT;
            pipe_index++;
        } else if (strcmp(tokens[i], ">") == 0) {
            order_of_pipes[pipe_index] = OUTPUT;
            pipe_index++;
        }
    }
}

void handle_pipe(char **tokens, int token_count, int pipe_count, int is_background) {
    generate_order(tokens, token_count);
}
