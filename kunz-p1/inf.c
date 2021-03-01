#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int i = 0;
    while (i < 5) {
        i++;
        //printf("%d\n", i);
        sleep(2);
    }
    return 0;
}
