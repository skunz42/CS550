#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("/dev/simple_misc", O_RDWR);
    char buffer[100];
    int n = read(fd, buffer, 100);
    printf("From device: %s\n", buffer);
    close(fd);
    return 0;
}
