#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd = open("/dev/process_list", O_RDWR);
    char buffer[10000];
    int bytes_read;
    bytes_read = read(fd, buffer, 10000);
    printf("%s", buffer);
    close(fd);
    return 0;
}
