#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "pipes.h"

#define VERTICAL 0
#define INPUT 1
#define OUTPUT 2

int order_of_pipes[100];
int pipe_order_size;

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

/*void generate_order(char **tokens, int token_count) {
    pipe_order_size = 0;
    for (int i = 0; i < token_count; i++) {
        if (strcmp(tokens[i], "|") == 0) {
            order_of_pipes[pipe_index] = VERTICAL;
            pipe_order_size++;
        } else if (strcmp(tokens[i], "<") == 0) {
            order_of_pipes[pipe_index] = INPUT;
            pipe_order_size++;
        } else if (strcmp(tokens[i], ">") == 0) {
            order_of_pipes[pipe_index] = OUTPUT;
            pipe_order_size++;
        }
    }
}*/

void handle_input(char ** commands, char * fname) { }

void handle_output(char ** commands, char * fname) {
    int out;

    out = open(fname, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    dup2(out, 1);
    close(out);
    execvp(commands[0], commands);
}

void handle_all_pipes(char **tokens, int token_count, int pipe_count, int is_background) {
    //generate_order(tokens, token_count);
    int last_pipe = 0;
    for (int i = 0; i < token_count; i++) {
        if (strcmp(tokens[i], "<") == 0 && i != token_count-1) {
            char * slice[i - last_pipe+1];
            int slice_i = 0;
            char fname[100];
            for (int j = last_pipe; j <= i; j++) {
                if (j == i) {
                    slice[slice_i] = NULL;
                    strcpy(fname, tokens[j+1]);
                } else {
                    slice[slice_i] = tokens[j];
                }
                slice_i++;
            }
            last_pipe = i+2;
            handle_input(slice, fname);
        } else if (strcmp(tokens[i], ">") == 0 && i != token_count-1) {
            char * slice[i - last_pipe+1];
            int slice_i = 0;
            char fname[100];
            for (int j = last_pipe; j <= i; j++) {
                if (j == i) {
                    slice[slice_i] = NULL;
                    strcpy(fname, tokens[j+1]);
                } else {
                    slice[slice_i] = tokens[j];
                }
                slice_i++;
            }
            last_pipe = i+2;
            handle_output(slice, fname);
        }
    }
}
