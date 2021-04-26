#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 10000

int main() {
    int fd = open("/dev/process_list", O_RDWR);
    char buffer[BUFFER_SIZE];
    int bytes_read;

    bytes_read = read(fd, buffer, BUFFER_SIZE);
    printf("%s", buffer);
    close(fd);
    return 0;
}
