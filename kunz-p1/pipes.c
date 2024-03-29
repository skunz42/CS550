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

char * pipe_sep[100][100];
int pipe_sep_size;

int *pipes;
char * saved_ifn;
char * saved_ofn;

int infdp;
int outfdp;

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
    pipes = malloc(sizeof(int) * num_pipe);
    for (int i = 0; i < num_pipe; i++) {
        pipe(pipes + (2*i));
    }
}

void generate_pipe_sep(char **tokens, int token_count) {
    int pipe_sep_idx = 0;
    int curr_idx = 0;
    for (int i = 0; i < token_count; i++) {
        if (strcmp(tokens[i], "|") == 0) {
            pipe_sep[pipe_sep_idx][curr_idx] = NULL;
            pipe_sep_idx++;
            curr_idx = 0;
        } else {
            pipe_sep[pipe_sep_idx][curr_idx] = tokens[i];
            curr_idx++;
        }
    }
    
    pipe_sep_size = pipe_sep_idx+1;
}

/** HANDLES JUST REDIRECTION **/
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

/** HANDLES PIPES + REDIRECTION **/
void handle_pipes(int cmd_num) {
    if (cmd_num == 0) {
        if (fork() == 0) {
            if (num_input) {
                for (int i = 0; i < 100; i++) {
                    if (pipe_sep[cmd_num][i] == NULL) {
                        break;
                    } else if (strcmp(pipe_sep[cmd_num][i], "<") == 0) {
                        saved_ifn = pipe_sep[cmd_num][i+1];
                        infdp = open(saved_ifn, O_RDONLY);
                        //pipes[0] = infdp;
                        //pipe_sep[cmd_num][i] = pipe_sep[cmd_num][i+1];
                        //pipe(pipes);
                        pipe_sep[cmd_num][i] = NULL;
                        pipe_sep[cmd_num][i+1] = NULL;
                        break;
                    }
                }
            }
            if (num_input) {
                dup2(infdp, 0);
                close(infdp);
            }
            dup2(pipes[cmd_num+1], 1);
            for (int j = 0; j < 2*num_pipe; j++) {
                close(pipes[j]);
            }
            execvp(pipe_sep[cmd_num][0], pipe_sep[cmd_num]);
        } else {
            handle_pipes(cmd_num+1);
        }
    } else if (cmd_num == pipe_sep_size-1) {
        if (num_output) {
            for (int i = 0; i < 100; i++) {
                if (pipe_sep[cmd_num][i] == NULL) {
                    break;
                } else if (strcmp(pipe_sep[cmd_num][i], ">") == 0) {
                    saved_ofn = pipe_sep[cmd_num][i+1];
                    outfdp = open(saved_ofn, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                    //pipes[2*num_pipe-1] = outfdp;
                    //pipe_sep[cmd_num][i] = pipe_sep[cmd_num][i+1];
                    //pipe(pipes);
                    pipe_sep[cmd_num][i] = NULL;
                    pipe_sep[cmd_num][i+1] = NULL;
                    break;
                }
            }
        }

        dup2(pipes[2*num_pipe-2], 0);
        if (num_output) {
            dup2(outfdp, 1);
            close(outfdp);
        }
        for (int j = 0; j < 2*num_pipe; j++) {
            close(pipes[j]);
        }
        execvp(pipe_sep[cmd_num][0], pipe_sep[cmd_num]);
    } else {
        if (fork() == 0) {
            dup2(pipes[2*cmd_num-2], 0);
            dup2(pipes[2*cmd_num+1], 1);
            for (int j = 0; j < 2*num_pipe; j++) {
                close(pipes[j]);
            }
            execvp(pipe_sep[cmd_num][0], pipe_sep[cmd_num]);
        } else {
            handle_pipes(cmd_num+1);
        }
    }
}

void handle_all_pipes(char **tokens, int token_count, int pipe_count, int is_background) {
    generate_order(tokens, token_count);
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
        generate_pipe_sep(tokens, token_count);
        handle_pipes(0);
    }
}
