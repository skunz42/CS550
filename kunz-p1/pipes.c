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

void handle_input(char ** tokens, int curr_index, int token_count) { 
    num_input--;
    //char *slice[curr_index+1];

    if (in_cmd[0] == NULL) {
        int slice_i = 0;
        char fname[100];
        int last_pipe = 0;
        for (int j = last_pipe; j <= curr_index; j++) {
            if (j == curr_index) {
                in_cmd[slice_i] = NULL;
                strcpy(fname, tokens[j+1]);
            } else {
                in_cmd[slice_i] = tokens[j];
            }
            slice_i++;
            last_pipe = curr_index+2;
        }
        infd = open(fname, O_RDONLY);
        dup2(infd, 0);
        close(infd);
        if (num_input == 0) {
            execvp(in_cmd[0], in_cmd);
        }
    } else {
        char fname[100];
        if (tokens[curr_index+1] != NULL) {
            strcpy(fname, tokens[curr_index+1]);
            infd = open(fname, O_RDONLY);
            dup2(infd, 0);
            close(infd);
            if (num_input == 0) {
                execvp(in_cmd[0], in_cmd);
            }
        }
    }
    /*char *slice[curr_index+1];
    int slice_i = 0;
    char fname[100];
    for (int j = 0; j <= curr_index; j++) {
        if (j == curr_index) {
            slice[slice_i] = NULL;
            strcpy(fname, tokens[j+1]);
        } else {
            slice[slice_i] = tokens[j];
        }
        slice_i++;
    }

    infd = open(fname, O_RDONLY);
    dup2(infd, 0);
    close(infd);
    execvp(slice[0], slice);*/
}

void handle_output(char ** tokens, int curr_index, int token_count) {
    num_output--;
    //char *slice[curr_index+1];

    if (out_cmd[0] == NULL) {
        int slice_i = 0;
        char fname[100];
        int last_pipe = 0;
        for (int j = last_pipe; j <= curr_index; j++) {
            if (j == curr_index) {
                out_cmd[slice_i] = NULL;
                strcpy(fname, tokens[j+1]);
            } else {
                out_cmd[slice_i] = tokens[j];
            }
            slice_i++;
            last_pipe = curr_index+2;
        }
        outfd = open(fname, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
        dup2(outfd, 1);
        close(outfd);
        if (num_output == 0) {
            execvp(out_cmd[0], out_cmd);
        }
    } else {
        char fname[100];
        if (tokens[curr_index+1] != NULL) {
            strcpy(fname, tokens[curr_index+1]);
            outfd = open(fname, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
            dup2(outfd, 1);
            close(outfd);
            if (num_output == 0) {
                execvp(out_cmd[0], out_cmd);
            }
        }
    }
}

void handle_all_pipes(char **tokens, int token_count, int pipe_count, int is_background) {
    generate_order(tokens, token_count);
    //int last_pipe = 0;
    for (int i = 0; i < token_count; i++) {
        if (num_pipe == 0) {
            if (num_input > 0 && num_output > 0) {


            } else if (num_input > 0) {
                if (strcmp(tokens[i], "<") == 0 && i != token_count-1) {
                    handle_input(tokens, i, token_count);
                }
            } else if (num_output > 0) {
                if (strcmp(tokens[i], ">") == 0 && i != token_count-1) {
                    handle_output(tokens, i, token_count);
                }
            }
        }
        
        
        
        
        
        
        
        /*if (strcmp(tokens[i], "<") == 0 && i != token_count-1) {
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
        }*/
    }
}
