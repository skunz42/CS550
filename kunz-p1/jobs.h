#include <stdlib.h>

typedef struct Job {
    char name[100];
    pid_t pid;
    int status; // 0 = RUNNING, 1 = FINISHED
    struct Job* next;
} Job;

typedef struct List {
    Job* head;
} List;

void listall(List *l);
void insert(List *l, Job* j);
void free_list(List *l);
