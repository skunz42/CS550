#include <stdlib.h>

struct job {
    char name[100];
    pid_t pid;
    int status;
    struct job* next;
};

struct list {
    struct job* head;
};

void listall();
