#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd = open("/dev/process_list", O_RDWR);
    char buffer[100];
    int bytes_read = 100;
    //while (bytes_read > 5) {
        bytes_read = read(fd, buffer, 100);
        printf("from dev: %s\n", buffer);
        printf("size of buffer: %d\n", bytes_read);
    //}
    close(fd);
    return 0;
}
