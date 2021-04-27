#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "proc_struct.h"

char * get_state(long int state_num) {
   switch (state_num) {
       case 0x0:
            return "TASK_RUNNING";
       case 0x1:
            return "TASK_INTERRUPTIBLE";
       case 0x2:
            return "TASK_UNINTERRUPTIBLE";
       case 0x4:
            return "__TASK_STOPPED";
       case 0x8:
            return "__TASK_TRACED";
       case 0x10:
            return "EXIT_DEAD";
       case 0x20:
            return "EXIT_ZOMBIE";
       case 0x40:
            return "TASK_PARKED";
       case 0x80:
            return "TASK_DEAD";
       case 0x100:
            return "TASK_WAKEKILL";
       case 0x200:
            return "TASK_WAKING";
       case 0x400:
            return "TASK_NOLOAD";
       case 0x800:
            return "TASK_NEW";
       case 0x1000:
            return "TASK_STATE_MAX";
       case (0x20 | 0x10):
            return "EXIT_ZOMBIE, EXIT_DEAD";
       case (0x100 | 0x2):
            return "TASK_WAKEKILL, TASK_UNINTERRUPTIBLE";
       case (0x100 | 0x4):
            return "TASK_WAKEKILL, __TASK_STOPPED";
       case (0x100 | 0x8):
            return "TASK_WAKEKILL, __TASK_TRACED";
       case (0x2 | 0x400):
            return "TASK_UNINTERRUPTIBLE, TASK_NOLOAD";
       case (0x2 | 0x1):
            return "TASK_INTERRUPTIBLE, TASK_UNINTERRUPTIBLE";
       case (0x0 | 0x1 | 0x2 | 0x4 | 0x8 | 0x10 | 0x20 | 0x40):
            return "TASK_RUNNING, TASK_INTERRUPTIBLE, TASK_UNINTERRUPTIBLE, TASK_STOPPED, __TASK_TRACED, EXIT_DEAD, EXIT_ZOMBIE, TASK_PARKED";
       default:
            return "UNKNOWN_STATE";
   }
}

int main() {
    int fd = open("/dev/process_list", O_RDWR);
    char *buffer = malloc(BUFFER_SIZE);
    int bytes_read = 1;
    proc_node *read_fr;

    if (fd < 0) {
        printf("Error opening device\n");
        return -1;
    }

    while (bytes_read != 0) {
        bytes_read = read(fd, buffer, BUFFER_SIZE);
        if (bytes_read == 0) {
            break;
        }
        if (bytes_read < 0) {
            printf("error reading bytes\n");
            return -1;
        }
        read_fr = (proc_node *)buffer;
        printf("PID=%d PPID=%d CPU=%d STATE=%s\n", read_fr->pid, read_fr->ppid, read_fr->cpu, get_state(read_fr->state));
    }
    free(buffer);
    close(fd);
    return 0;
}
