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

int num_input = 0;
int num_output = 0;
int num_pipe = 0;

char * out_cmd[100];
char * in_cmd[100];
char * io_cmd[100];

int outfd;
int infd;

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
    for (int i = 0; i < token_count; i++) {
        if (strcmp(tokens[i], "<") == 0) {
            num_input++;
        } else if (strcmp(tokens[i], ">") == 0) {
            num_output++;
        } else if (strcmp(tokens[i], "|") == 0) {
            num_pipe++;
        }
    }
}

void handle_io(char **tokens, int curr_index, int token_count) {
    if (io_cmd[0] == NULL) {
        int slice_i = 0;
        char fname[100];
        int last_pipe = 0;
        for (int j = last_pipe; j <= curr_index; j++) {
            if (j == curr_index) {
                if (strcmp(tokens[j], "<") == 0) {
                    num_input--;
                } else if (strcmp(tokens[j], ">") == 0) {
                    num_output--;
                }
                io_cmd[slice_i] = NULL;
                strcpy(fname, tokens[j+1]);
            } else {
                io_cmd[slice_i] = tokens[j];
            }
            slice_i++;
            last_pipe = curr_index+2;
        }
        if (strcmp(tokens[curr_index], "<") == 0) {
            infd = open(fname, O_RDONLY);
            dup2(infd, 0);
            close(infd);
        } else if (strcmp(tokens[curr_index], ">") == 0) {
             outfd = open(fname, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
            dup2(outfd, 1);
            close(outfd);
        }
        if (num_input == 0 && num_output == 0) {
            execvp(io_cmd[0], io_cmd);
        }
    } else {
        char ifname[100];
        char ofname[100];
        if (tokens[curr_index+1] != NULL) {
            if (strcmp(tokens[curr_index], "<") == 0) {
                num_input--;
                strcpy(ifname, tokens[curr_index+1]);
                infd = open(ifname, O_RDONLY);
                dup2(infd, 0);
                close(infd);
            } else if (strcmp(tokens[curr_index], ">") == 0) {
                num_output--;
                strcpy(ofname, tokens[curr_index+1]);
                outfd = open(ofname, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                dup2(outfd, 1);
                close(outfd);
            }
            if (num_input == 0 && num_output == 0) {
                execvp(io_cmd[0], io_cmd);
            }
        }                
    }
}

void handle_all_pipes(char **tokens, int token_count, int pipe_count, int is_background) {
    generate_order(tokens, token_count);
    //int last_pipe = 0;
    if (num_pipe == 0) {
        for (int i = 0; i < token_count; i++) {
            if (num_input > 0 || num_output > 0) {
                if ((strcmp(tokens[i], "<") == 0 || strcmp(tokens[i], ">") == 0) && i != token_count-1) {
                    handle_io(tokens, i, token_count);
                }
            }
        }
    } else {
        // if pipes, run each chunk of command recursively
    }
}
