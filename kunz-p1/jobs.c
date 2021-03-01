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

            if (runner->status == 0) {
                printf("%s with PID: %d Status: RUNNING\n", runner->name, runner->pid);
            } else {
                printf("%s with PID: %d Status: FINISHED\n", runner->name, runner->pid);
            }
            runner = runner->next;
        }
    }
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
