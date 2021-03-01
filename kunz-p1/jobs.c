#include <stdio.h>
#include <stdlib.h>

#include "jobs.h"

void listall(List *l) {
    printf("List of background processes:\n");
    if (l->head != NULL) {
        Job *runner = l->head;
        while (runner != NULL) {
            if (runner == NULL) {
                break;
            }
            printf("PID: %d\n", runner->pid);
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
