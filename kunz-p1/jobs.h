#include <stdlib.h>

//PID status
#define RUNNING 0
#define FINISHED 1

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
void foreground(List *l, pid_t pid);
void check_finished(List *l);
