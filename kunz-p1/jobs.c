#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "jobs.h"

void listall(List *l) {
    int wait_ret;
    int cstat;

    printf("List of background processes:\n");
    if (l->head != NULL) {
        Job *runner = l->head;
        while (runner != NULL) {
            if (runner == NULL) {
                break;
            }

            if ((wait_ret = waitpid(runner->pid, &cstat, WNOHANG)) == -1) {
                runner->status = FINISHED;
            }

            if (runner->status == RUNNING) {
                printf("%s with PID: %d Status: RUNNING\n", runner->name, runner->pid);
            } else {
                printf("%s with PID: %d Status: FINISHED\n", runner->name, runner->pid);
            }
            runner = runner->next;
        }
    }
}

void check_finished(List *l) {
    int wait_ret;
    int status;

    if (l->head != NULL) {
        Job *runner = l->head;
        while (runner != NULL) {
            if (runner == NULL) {
                break;
            }        

            if ((wait_ret = waitpid(runner->pid, &status, WNOHANG)) == -1) {
                runner->status = FINISHED;
            }
            runner = runner->next;
        }
    }
}

void foreground(List *l, pid_t pid) {
    int wait_ret;
    int status;
    if (l->head != NULL) {
        Job * runner = l->head;
        while (runner != NULL) {
            if (runner == NULL) {
                break;
            }
            if (runner->pid == pid) {
                wait_ret = waitpid(runner->pid, &status, 0);
                if (wait_ret < 0) {
                    break;
                }
                return;
            }
            runner = runner->next;
        }
    }
    printf("PID does not exist\n");
}


void insert(List *l, Job * j) {
    if (l->head == NULL) {
        l->head = j;
    } else {
        Job *runner = l->head;
        while (runner != NULL) {
            if (runner->next == NULL) {
                runner->next = j;
                break;
            }
            runner = runner->next;
        }
    }
}

void free_list(List *l) {
    if (l->head != NULL) {
        Job *runner = l->head;
        while (runner != NULL) {
            if (runner == NULL) {
                break;
            }
            Job *prev = runner;
            runner = runner->next;
            free(prev);
        }
    }
}
