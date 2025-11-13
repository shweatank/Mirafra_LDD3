#include <stdio.h>      // printf
#include <stdlib.h>     // exit
#include <sys/time.h>   // gettimeofday

int main() {
    struct timeval tv; // Structure to hold seconds + microseconds

    // Get current system (wall-clock) time
    if (gettimeofday(&tv, NULL) == -1) {
        perror("gettimeofday failed");
        exit(1);
    }

    printf("Seconds since Epoch: %ld\n", tv.tv_sec);
    printf("Microseconds: %ld\n", tv.tv_usec);

    return 0;
}

