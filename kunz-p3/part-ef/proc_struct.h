typedef struct proc_node {
    int pid;
    int ppid;
    int cpu;
    long int state;
    struct proc_node *next;
} proc_node;

#define BUFFER_SIZE sizeof(proc_node)
