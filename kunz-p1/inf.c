#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int i = 0;
    while (1) {
        i++;
        printf("%d\n", i);
        sleep(2);
    }
    return 0;
}
