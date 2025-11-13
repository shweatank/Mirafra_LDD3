#include <stdio.h>      // printf
#include <time.h>       // time, ctime

int main() {
    time_t raw_time;  // Holds calendar time in seconds
    time(&raw_time);  // Get current system time

    printf("Current time: %s", ctime(&raw_time));
    return 0;
}

